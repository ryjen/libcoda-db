#include "postgres_statement.h"
#include "postgres_db.h"
#include "postgres_resultset.h"

#ifdef HAVE_LIBPQ

namespace arg3
{
    namespace db
    {
        postgres_statement::postgres_statement(postgres_db *db) : db_(db), stmt_(nullptr)
        {
            if (db_ == NULL) {
                throw database_exception("no database provided to postgres statement");
            }
        }

        postgres_statement::postgres_statement(postgres_statement &&other) : db_(other.db_), stmt_(other.stmt_)
        {
            other.stmt_ = nullptr;
            other.db_ = NULL;
        }

        postgres_statement &postgres_statement::operator=(postgres_statement &&other)
        {
            db_ = other.db_;
            stmt_ = other.stmt_;

            other.stmt_ = nullptr;
            other.db_ = NULL;

            return *this;
        }

        postgres_statement::~postgres_statement()
        {
            stmt_ = nullptr;
        }

        void postgres_statement::prepare(const string &sql)
        {
            if (!db_ || !db_->db_) {
                throw database_exception("database not open");
            }

            sql_ = sql;
        }

        bool postgres_statement::is_valid() const
        {
            return !sql_.empty();
        }

        int postgres_statement::last_number_of_changes()
        {
            char *changes = PQcmdTuples(stmt_.get());

            if (changes == nullptr || *changes == 0) {
                return 0;
            }

            try {
                return stoi(changes);
            } catch (const std::exception &e) {
                return 0;
            }
        }

        string postgres_statement::last_error()
        {
            return db_->last_error();
        }

        postgres_statement &postgres_statement::bind(size_t index, int value)
        {
            bindings_.bind(index, value);
            return *this;
        }
        postgres_statement &postgres_statement::bind(size_t index, long long value)
        {
            bindings_.bind(index, value);
            return *this;
        }
        postgres_statement &postgres_statement::bind(size_t index, double value)
        {
            bindings_.bind(index, value);
            return *this;
        }
        postgres_statement &postgres_statement::bind(size_t index, const std::string &value, int len)
        {
            bindings_.bind(index, value, len);
            return *this;
        }
        postgres_statement &postgres_statement::bind(size_t index, const std::wstring &value, int len)
        {
            bindings_.bind(index, value, len);
            return *this;
        }
        postgres_statement &postgres_statement::bind(size_t index, const sql_blob &value)
        {
            bindings_.bind(index, value);
            return *this;
        }

        postgres_statement &postgres_statement::bind(size_t index, const sql_null_t &value)
        {
            bindings_.bind(index, value);
            return *this;
        }

        resultset postgres_statement::results()
        {
            PGresult *res = PQexecParams(db_->db_.get(), sql_.c_str(), bindings_.size(), bindings_.types_, bindings_.values_, bindings_.lengths_,
                                         bindings_.formats_, 0);

            if (PQresultStatus(res) != PGRES_TUPLES_OK) {
                throw database_exception(last_error());
            }

            stmt_ = shared_ptr<PGresult>(res, helper::postgres_res_delete());

            if (db_->cache_level() == sqldb::CACHE_RESULTSET)
                return resultset(make_shared<postgres_cached_resultset>(db_, stmt_));
            else
                return resultset(make_shared<postgres_resultset>(db_, stmt_));
        }

        bool postgres_statement::result()
        {
            PGresult *res = PQexecParams(db_->db_.get(), sql_.c_str(), bindings_.size(), bindings_.types_, bindings_.values_, bindings_.lengths_,
                                         bindings_.formats_, 0);

            if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                return false;
            }

            stmt_ = shared_ptr<PGresult>(res, helper::postgres_res_delete());

            return true;
        }

        void postgres_statement::finish()
        {
            stmt_ = nullptr;
            sql_.clear();
        }

        void postgres_statement::reset()
        {
            if (stmt_ != nullptr) {
                PQclear(stmt_.get());
            }
            sql_.clear();
        }

        long long postgres_statement::last_insert_id()
        {
            return PQoidValue(stmt_.get());
        }
    }
}

#endif

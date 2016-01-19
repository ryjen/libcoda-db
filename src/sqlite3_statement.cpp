#include "sqlite3_statement.h"
#include "sqlite3_db.h"
#include "sqlite3_resultset.h"

#ifdef HAVE_LIBSQLITE3

namespace arg3
{
    namespace db
    {
        void sqlite3_stmt_delete::operator()(sqlite3_stmt *p) const
        {
            if (p != NULL) {
                sqlite3_finalize(p);
            }
        }

        sqlite3_statement::sqlite3_statement(sqlite3_db *db) : db_(db), stmt_(nullptr)
        {
            if (db_ == NULL) {
                throw database_exception("no database provided to sqlite3 statement");
            }
        }

        sqlite3_statement::sqlite3_statement(sqlite3_statement &&other) : db_(other.db_), stmt_(other.stmt_)
        {
            other.stmt_ = nullptr;
            other.db_ = NULL;
        }

        sqlite3_statement &sqlite3_statement::operator=(sqlite3_statement &&other)
        {
            db_ = other.db_;
            stmt_ = other.stmt_;

            other.stmt_ = nullptr;
            other.db_ = NULL;

            return *this;
        }

        sqlite3_statement::~sqlite3_statement()
        {
        }

        void sqlite3_statement::prepare(const string &sql)
        {
            if (!db_ || !db_->db_) {
                throw database_exception("database not open");
            }

            if (is_valid()) return;

            sqlite3_stmt *temp;

            if (sqlite3_prepare_v2(db_->db_.get(), sql.c_str(), -1, &temp, NULL) != SQLITE_OK) {
                throw database_exception(db_->last_error());
            }

            stmt_ = shared_ptr<sqlite3_stmt>(temp, sqlite3_stmt_delete());

            return;
        }

        bool sqlite3_statement::is_valid() const
        {
            return stmt_ != nullptr && stmt_;
        }

        int sqlite3_statement::last_number_of_changes()
        {
            return db_->last_number_of_changes();
        }

        string sqlite3_statement::last_error()
        {
            return db_->last_error();
        }

        sqlite3_statement &sqlite3_statement::bind(size_t index, int value)
        {
            if (sqlite3_bind_int(stmt_.get(), index, value) != SQLITE_OK) {
                throw binding_error(db_->last_error());
            }
            return *this;
        }
        sqlite3_statement &sqlite3_statement::bind(size_t index, long long value)
        {
            if (sqlite3_bind_int64(stmt_.get(), index, value) != SQLITE_OK) {
                throw binding_error(db_->last_error());
            }
            return *this;
        }
        sqlite3_statement &sqlite3_statement::bind(size_t index, double value)
        {
            if (sqlite3_bind_double(stmt_.get(), index, value) != SQLITE_OK) {
                throw binding_error(db_->last_error());
            }
            return *this;
        }
        sqlite3_statement &sqlite3_statement::bind(size_t index, const std::string &value, int len)
        {
            if (sqlite3_bind_text(stmt_.get(), index, value.c_str(), len, SQLITE_TRANSIENT) != SQLITE_OK) {
                throw binding_error(db_->last_error());
            }
            return *this;
        }
        sqlite3_statement &sqlite3_statement::bind(size_t index, const std::wstring &value, int len)
        {
            if (sqlite3_bind_text16(stmt_.get(), index, value.c_str(), len, SQLITE_TRANSIENT) != SQLITE_OK) {
                throw binding_error(db_->last_error());
            }
            return *this;
        }
        sqlite3_statement &sqlite3_statement::bind(size_t index, const sql_blob &value)
        {
            if (sqlite3_bind_blob(stmt_.get(), index, value.value(), value.size(), value.is_transient() ? SQLITE_TRANSIENT : SQLITE_STATIC) !=
                SQLITE_OK) {
                throw binding_error(db_->last_error());
            }
            return *this;
        }

        sqlite3_statement &sqlite3_statement::bind(size_t index, const sql_null_t &value)
        {
            if (sqlite3_bind_null(stmt_.get(), index) != SQLITE_OK) {
                throw binding_error(db_->last_error());
            }
            return *this;
        }

        resultset sqlite3_statement::results()
        {
            if (db_->cache_level() == sqldb::CACHE_RESULTSET)
                return resultset(make_shared<sqlite3_cached_resultset>(db_, stmt_));
            else
                return resultset(make_shared<sqlite3_resultset>(db_, stmt_));
        }

        bool sqlite3_statement::result()
        {
            return sqlite3_step(stmt_.get()) == SQLITE_DONE;
        }

        void sqlite3_statement::finish()
        {
            stmt_ = nullptr;
        }

        void sqlite3_statement::reset()
        {
            if (sqlite3_reset(stmt_.get()) != SQLITE_OK) {
                throw database_exception(db_->last_error());
            }
        }

        long long sqlite3_statement::last_insert_id()
        {
            return db_->last_insert_id();
        }
    }
}

#endif

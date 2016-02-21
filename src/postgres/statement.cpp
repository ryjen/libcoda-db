
#include <algorithm>
#include "statement.h"
#include "db.h"
#include "resultset.h"
#include "../log.h"

#ifdef HAVE_LIBPQ

using namespace std;

namespace arg3
{
    namespace db
    {
        namespace postgres
        {
            statement::statement(postgres::db *db) : db_(db), stmt_(nullptr)
            {
                if (db_ == nullptr) {
                    throw database_exception("no database provided to postgres statement");
                }
            }

            statement::statement(statement &&other)
                : db_(other.db_), stmt_(std::move(other.stmt_)), bindings_(std::move(other.bindings_)), sql_(std::move(other.sql_))
            {
                other.stmt_ = nullptr;
                other.db_ = nullptr;
            }

            statement &statement::operator=(statement &&other)
            {
                db_ = other.db_;
                stmt_ = std::move(other.stmt_);
                bindings_ = std::move(other.bindings_);
                sql_ = std::move(other.sql_);

                other.stmt_ = nullptr;
                other.db_ = nullptr;
                other.sql_.clear();

                return *this;
            }

            statement::~statement()
            {
                stmt_ = nullptr;
            }

            void statement::prepare(const string &sql)
            {
                static std::string insert("INSERT");

                if (!db_ || !db_->is_open()) {
                    throw database_exception("postgres database not open");
                }

                sql_ = sql;
            }

            bool statement::is_valid() const
            {
                return !sql_.empty();
            }

            int statement::last_number_of_changes()
            {
                if (stmt_ == nullptr) {
                    return 0;
                }
                char *changes = PQcmdTuples(stmt_.get());

                int value = 0;

                if (changes != nullptr && *changes != 0) {
                    try {
                        value = stoi(changes);
                    } catch (const std::exception &e) {
                        value = 0;
                    }
                }
                if (db_ != nullptr) {
                    db_->set_last_number_of_changes(value);
                }

                return value;
            }

            string statement::last_error()
            {
                if (db_ == nullptr) {
                    return "no database";
                }
                return db_->last_error();
            }

            statement &statement::bind(size_t index, int value)
            {
                bindings_.bind(index, value);
                return *this;
            }
            statement &statement::bind(size_t index, unsigned value)
            {
                bindings_.bind(index, value);
                return *this;
            }
            statement &statement::bind(size_t index, long long value)
            {
                bindings_.bind(index, value);
                return *this;
            }
            statement &statement::bind(size_t index, unsigned long long value)
            {
                bindings_.bind(index, value);
                return *this;
            }
            statement &statement::bind(size_t index, float value)
            {
                bindings_.bind(index, value);
                return *this;
            }
            statement &statement::bind(size_t index, double value)
            {
                bindings_.bind(index, value);
                return *this;
            }
            statement &statement::bind(size_t index, const std::string &value, int len)
            {
                bindings_.bind(index, value, len);
                return *this;
            }
            statement &statement::bind(size_t index, const std::wstring &value, int len)
            {
                bindings_.bind(index, value, len);
                return *this;
            }
            statement &statement::bind(size_t index, const sql_blob &value)
            {
                bindings_.bind(index, value);
                return *this;
            }

            statement &statement::bind(size_t index, const sql_null_type &value)
            {
                bindings_.bind(index, value);
                return *this;
            }
            statement &statement::bind(size_t index, const sql_time &value)
            {
                bindings_.bind(index, value);
                return *this;
            }
            statement::resultset_type statement::results()
            {
                PGresult *res = PQexecParams(db_->db_.get(), sql_.c_str(), bindings_.size(), bindings_.types_, bindings_.values_, bindings_.lengths_,
                                             bindings_.formats_, 0);

                if (PQresultStatus(res) != PGRES_TUPLES_OK) {
                    throw database_exception(last_error());
                }

                stmt_ = shared_ptr<PGresult>(res, helper::res_delete());

                return resultset_type(make_shared<resultset>(db_, stmt_));
            }

            bool statement::result()
            {
                PGresult *res = PQexecParams(db_->db_.get(), sql_.c_str(), bindings_.size(), bindings_.types_, bindings_.values_, bindings_.lengths_,
                                             bindings_.formats_, 0);

                if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
                    log::error(last_error().c_str());
                    return false;
                }

                stmt_ = shared_ptr<PGresult>(res, helper::res_delete());

                return true;
            }

            void statement::finish()
            {
                stmt_ = nullptr;
                sql_.clear();
            }

            void statement::reset()
            {
                if (stmt_ != nullptr) {
                    PQclear(stmt_.get());
                }
                sql_.clear();
            }

            long long statement::last_insert_id()
            {
                Oid oid = PQoidValue(stmt_.get());

                long long value = 0;

                if (oid == InvalidOid) {
                    auto val = PQgetvalue(stmt_.get(), 0, 0);
                    if (val != nullptr) {
                        try {
                            value = stoll(val);
                        } catch (const std::exception &e) {
                            value = 0;
                        }
                    }
                }

                db_->set_last_insert_id(value);

                return value;
            }
        }
    }
}

#endif

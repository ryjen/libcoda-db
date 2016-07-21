
#include "statement.h"
#include <algorithm>
#include "../log.h"
#include "resultset.h"
#include "session.h"

#ifdef HAVE_LIBPQ

using namespace std;

namespace rj
{
    namespace db
    {
        namespace postgres
        {
            namespace helper
            {
                void res_delete::operator()(PGresult *p) const
                {
                    if (p != nullptr) {
                        PQclear(p);
                    }
                }
            }
            statement::statement(const std::shared_ptr<postgres::session> &sess) : sess_(sess), stmt_(nullptr)
            {
                if (sess_ == nullptr) {
                    throw database_exception("no database provided to postgres statement");
                }
            }

            statement::statement(statement &&other)
                : sess_(std::move(other.sess_)), stmt_(std::move(other.stmt_)), bindings_(std::move(other.bindings_)), sql_(std::move(other.sql_))
            {
                other.stmt_ = nullptr;
                other.sess_ = nullptr;
            }

            statement &statement::operator=(statement &&other)
            {
                sess_ = std::move(other.sess_);
                stmt_ = std::move(other.stmt_);
                bindings_ = std::move(other.bindings_);
                sql_ = std::move(other.sql_);

                other.stmt_ = nullptr;
                other.sess_ = nullptr;
                other.sql_.clear();

                return *this;
            }

            statement::~statement()
            {
            }

            void statement::prepare(const string &sql)
            {
                if (!sess_ || !sess_->is_open()) {
                    throw database_exception("postgres database not open");
                }

                sql_ = bindings_.prepare(sql);
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
                if (sess_ != nullptr) {
                    sess_->set_last_number_of_changes(value);
                }

                return value;
            }

            string statement::last_error()
            {
                if (sess_ == nullptr) {
                    return "no database";
                }
                return sess_->last_error();
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

            statement &statement::bind(const string &name, const sql_value &value)
            {
                bindings_.bind(name, value);
                return *this;
            }

            statement::resultset_type statement::results()
            {
                if (sess_ == nullptr) {
                    throw database_exception("statement::results invalid database");
                }

                PGresult *res = PQexecParams(sess_->db_.get(), sql_.c_str(), bindings_.size(), bindings_.types_, bindings_.values_,
                                             bindings_.lengths_, bindings_.formats_, 0);

                if (PQresultStatus(res) != PGRES_TUPLES_OK) {
                    throw database_exception(last_error());
                }

                stmt_ = shared_ptr<PGresult>(res, helper::res_delete());

                return resultset_type(make_shared<resultset>(sess_, stmt_));
            }

            bool statement::result()
            {
                if (sess_ == nullptr) {
                    throw database_exception("statement::results invalid database");
                }

                PGresult *res = PQexecParams(sess_->db_.get(), sql_.c_str(), bindings_.size(), bindings_.types_, bindings_.values_,
                                             bindings_.lengths_, bindings_.formats_, 0);

                if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
                    PQclear(res);
                    log::error("%s", last_error().c_str());
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
                    stmt_ = nullptr;
                }
            }

            long long statement::last_insert_id()
            {
                if (stmt_ == nullptr) {
                    return 0;
                }

                Oid oid = PQoidValue(stmt_.get());

                if (oid != InvalidOid) {
                    return oid;
                }

                long long value = 0;

                if (PQntuples(stmt_.get()) <= 0) {
                    return value;
                }
                auto val = PQgetvalue(stmt_.get(), 0, 0);
                if (val != nullptr) {
                    try {
                        value = stoll(val);
                    } catch (const std::exception &e) {
                        value = 0;
                    }
                }

                sess_->set_last_insert_id(value);

                return value;
            }
        }
    }
}

#endif

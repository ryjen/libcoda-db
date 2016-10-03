#include "statement.h"
#include "../log.h"
#include "resultset.h"
#include "session.h"

#ifdef HAVE_LIBSQLITE3

using namespace std;

namespace rj
{
    namespace db
    {
        namespace sqlite
        {
            namespace helper
            {
                void stmt_delete::operator()(sqlite3_stmt *p) const
                {
                    if (p != NULL) {
                        sqlite3_finalize(p);
                    }
                }
            }

            namespace data_mapper
            {
                class from_number : public boost::static_visitor<bool>
                {
                   public:
                    from_number(const std::shared_ptr<sqlite3_stmt> &stmt, size_t index) : stmt_(stmt), index_(index)
                    {
                    }

                    bool operator()(double value) const
                    {
                        return sqlite3_bind_double(stmt_.get(), index_, value) == SQLITE_OK;
                    }
                    bool operator()(long double value) const
                    {
                        return false;
                    }
                    bool operator()(float value) const
                    {
                        return sqlite3_bind_double(stmt_.get(), index_, value) == SQLITE_OK;
                    }

                    bool operator()(unsigned long long value) const
                    {
                        return false;
                    }

                    bool operator()(long long value) const
                    {
                        return sqlite3_bind_int64(stmt_.get(), index_, value) == SQLITE_OK;
                    }

                    bool operator()(unsigned int value) const
                    {
                        return sqlite3_bind_int64(stmt_.get(), index_, value) == SQLITE_OK;
                    }
                    bool operator()(int value) const
                    {
                        return sqlite3_bind_int(stmt_.get(), index_, value) == SQLITE_OK;
                    }
                    bool operator()(unsigned long value) const
                    {
                        return sqlite3_bind_int64(stmt_.get(), index_, value) == SQLITE_OK;
                    }
                    bool operator()(long value) const
                    {
                        return sqlite3_bind_int(stmt_.get(), index_, value) == SQLITE_OK;
                    }
                    bool operator()(const sql_null_type &value) const
                    {
                        return sqlite3_bind_null(stmt_.get(), index_);
                    }

                   private:
                    std::shared_ptr<sqlite3_stmt> stmt_;
                    size_t index_;
                };
                class from_value : public boost::static_visitor<bool>
                {
                   public:
                    from_value(const std::shared_ptr<sqlite3_stmt> &stmt, size_t index) : stmt_(stmt), index_(index)
                    {
                    }

                    bool operator()(int value) const
                    {
                        return sqlite3_bind_int(stmt_.get(), index_, value) == SQLITE_OK;
                    }

                    bool operator()(const sql_time &value) const
                    {
                        auto tstr = value.to_string();

                        return sqlite3_bind_text(stmt_.get(), index_, tstr.c_str(), tstr.size(), SQLITE_TRANSIENT) == SQLITE_OK;
                    }

                    bool operator()(const sql_null_type &value) const
                    {
                        return sqlite3_bind_null(stmt_.get(), index_) == SQLITE_OK;
                    }

                    bool operator()(const sql_blob &value) const
                    {
                        return sqlite3_bind_blob(stmt_.get(), index_, value.data(), value.size(), SQLITE_TRANSIENT) == SQLITE_OK;
                    }

                    bool operator()(const std::wstring &value) const
                    {
                        return sqlite3_bind_text16(stmt_.get(), index_, value.c_str(), -1, SQLITE_TRANSIENT) == SQLITE_OK;
                    }

                    bool operator()(const std::string &value) const
                    {
                        return sqlite3_bind_text(stmt_.get(), index_, value.c_str(), -1, SQLITE_TRANSIENT) == SQLITE_OK;
                    }

                    bool operator()(const sql_number &value) const
                    {
                        return value.apply_visitor<from_number, bool>(from_number(stmt_, index_));
                    }

                   private:
                    std::shared_ptr<sqlite3_stmt> stmt_;
                    size_t index_;
                };
            }

            statement::statement(const std::shared_ptr<sqlite::session> &sess) : sess_(sess), stmt_(nullptr)
            {
                if (sess_ == NULL) {
                    throw database_exception("no database provided to sqlite3 statement");
                }
            }

            statement::statement(statement &&other) : sess_(std::move(other.sess_)), stmt_(std::move(other.stmt_))
            {
                other.stmt_ = nullptr;
                other.sess_ = NULL;
            }

            statement &statement::operator=(statement &&other)
            {
                sess_ = std::move(other.sess_);
                stmt_ = std::move(other.stmt_);

                other.stmt_ = nullptr;
                other.sess_ = NULL;

                return *this;
            }

            statement::~statement()
            {
            }

            void statement::prepare(const string &sql)
            {
                if (!sess_ || !sess_->db_) {
                    throw database_exception("database not open");
                }

                if (is_valid()) {
                    log::warn("sql statement prepare not valid");
                    return;
                }

                sqlite3_stmt *temp;

                if (sqlite3_prepare_v2(sess_->db_.get(), sql.c_str(), -1, &temp, NULL) != SQLITE_OK) {
                    throw database_exception(sess_->last_error());
                }

                stmt_ = shared_ptr<sqlite3_stmt>(temp, helper::stmt_delete());
            }

            bool statement::is_valid() const
            {
                return stmt_ != nullptr && stmt_;
            }

            int statement::last_number_of_changes()
            {
                return sess_->last_number_of_changes();
            }

            string statement::last_error()
            {
                return sess_->last_error();
            }

            statement &statement::bind(size_t index, const sql_value &value)
            {
                if (!is_valid()) {
                    throw binding_error("statment invalid");
                }

                if (!value.apply_visitor<data_mapper::from_value, bool>(data_mapper::from_value(stmt_, index))) {
                    throw binding_error(sess_->last_error());
                }
                return *this;
            }

            statement &statement::bind(const string &name, const sql_value &value)
            {
                if (!is_valid()) {
                    throw binding_error("statment invalid");
                }

                int index = sqlite3_bind_parameter_index(stmt_.get(), name.c_str());

                if (index <= 0) {
                    throw binding_error("No such named parameter '" + name + "'");
                }

                bind(index, value);
                return *this;
            }

            statement::resultset_type statement::results()
            {
                if (sess_ == nullptr) {
                    throw database_exception("sqlite statement results invalid database");
                }

                return resultset_type(make_shared<resultset>(sess_, stmt_));
            }

            bool statement::result()
            {
                if (!is_valid()) {
                    log::warn("sqlite statement result invalid");
                    return false;
                }
                return sqlite3_step(stmt_.get()) == SQLITE_DONE;
            }

            void statement::finish()
            {
                stmt_ = nullptr;
            }

            void statement::reset()
            {
                if (!is_valid()) {
                    log::warn("sqlite statement reset invalid");
                    return;
                }
                if (sqlite3_reset(stmt_.get()) != SQLITE_OK) {
                    throw database_exception(sess_->last_error());
                }
            }

            long long statement::last_insert_id()
            {
                if (sess_ == nullptr) {
                    return 0;
                }
                return sess_->last_insert_id();
            }
        }
    }
}

#endif

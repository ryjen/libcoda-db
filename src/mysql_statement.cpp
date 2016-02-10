#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBMYSQLCLIENT

#include "mysql_statement.h"
#include "mysql_db.h"
#include "mysql_resultset.h"
#include <regex>

namespace arg3
{
    namespace db
    {
        namespace helper
        {
            extern string last_stmt_error(MYSQL_STMT *stmt);

            struct mysql_stmt_delete {
                void operator()(MYSQL_STMT *p) const
                {
                    mysql_stmt_close(p);
                }
            };
        }

        mysql_statement::mysql_statement(mysql_db *db) : db_(db), stmt_(nullptr)
        {
            if (db_ == nullptr) {
                throw database_exception("No database provided for mysql statement");
            }
        }

        mysql_statement::mysql_statement(mysql_statement &&other)
        {
            db_ = other.db_;
            stmt_ = other.stmt_;
            other.db_ = nullptr;
            other.stmt_ = nullptr;
        }

        mysql_statement &mysql_statement::operator=(mysql_statement &&other)
        {
            db_ = other.db_;
            stmt_ = other.stmt_;

            other.db_ = nullptr;
            other.stmt_ = nullptr;

            return *this;
        }

        mysql_statement::~mysql_statement()
        {
            finish();
        }

        void mysql_statement::prepare(const string &sql)
        {
            static regex param_regex("\\$([0-9]+)([:]{2}[a-z]+)?");
            static string param_repl("?");

            if (db_ == nullptr || !db_->is_open()) {
                throw database_exception("database is not open");
            }

            // TODO: this needs to be fleshed out into being able to handle duplicate placeholders
            // example: col1 = $1 AND col2 = $1
            string formatted_sql = regex_replace(sql, param_regex, param_repl);

            MYSQL_STMT *temp = mysql_stmt_init(db_->db_.get());

            if (temp == nullptr) {
                throw database_exception("out of memory");
            }

            stmt_ = shared_ptr<MYSQL_STMT>(temp, helper::mysql_stmt_delete());

            if (mysql_stmt_prepare(stmt_.get(), formatted_sql.c_str(), formatted_sql.length())) {
                throw database_exception(db_->last_error());
            }
        }

        bool mysql_statement::is_valid() const
        {
            return stmt_ != nullptr && stmt_;
        }

        /**
         * binding methods ensure the dynamic array is sized properly and store the value as a memory pointer
         */

        mysql_statement &mysql_statement::bind(size_t index, int value)
        {
            bindings_.bind(index, value);

            return *this;
        }
        mysql_statement &mysql_statement::bind(size_t index, long long value)
        {
            bindings_.bind(index, value);

            return *this;
        }
        mysql_statement &mysql_statement::bind(size_t index, double value)
        {
            bindings_.bind(index, value);
            return *this;
        }
        mysql_statement &mysql_statement::bind(size_t index, const std::string &value, int len)
        {
            bindings_.bind(index, value, len);
            return *this;
        }

        mysql_statement &mysql_statement::bind(size_t index, const std::wstring &value, int len)
        {
            bindings_.bind(index, value, len);
            return *this;
        }

        mysql_statement &mysql_statement::bind(size_t index, const sql_blob &value)
        {
            bindings_.bind(index, value);

            return *this;
        }
        mysql_statement &mysql_statement::bind(size_t index, const sql_null_type &value)
        {
            bindings_.bind(index, value);

            return *this;
        }

        resultset mysql_statement::results()
        {
            if (!is_valid()) {
                throw database_exception("statement not ready");
            }

            bindings_.bind_params(stmt_.get());

            return resultset(make_shared<mysql_stmt_resultset>(db_, stmt_));
        }

        bool mysql_statement::result()
        {
            if (!is_valid()) {
                return false;
            }

            bindings_.bind_params(stmt_.get());

            if (mysql_stmt_execute(stmt_.get())) {
                return false;
            }
            return true;
        }

        int mysql_statement::last_number_of_changes()
        {
            if (!is_valid()) {
                return 0;
            }

            return mysql_stmt_affected_rows(stmt_.get());
        }

        string mysql_statement::last_error()
        {
            if (!is_valid()) {
                throw database_exception("statement not ready");
            }

            return helper::last_stmt_error(stmt_.get());
        }

        void mysql_statement::finish()
        {
            bindings_.reset();

            if (stmt_ != nullptr) {
                mysql_stmt_free_result(stmt_.get());
                stmt_ = nullptr;
            }
        }

        void mysql_statement::reset()
        {
            bindings_.reset();

            if (!is_valid()) return;

            if (mysql_stmt_reset(stmt_.get())) {
                throw database_exception(last_error());
            }
        }

        long long mysql_statement::last_insert_id()
        {
            if (!is_valid()) return 0;

            return mysql_stmt_insert_id(stmt_.get());
        }
    }
}

#endif

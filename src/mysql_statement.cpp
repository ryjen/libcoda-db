#include "config.h"


#ifdef HAVE_LIBMYSQLCLIENT

#include "mysql_statement.h"
#include "mysql_db.h"
#include "mysql_resultset.h"

namespace arg3
{
    namespace db
    {

        mysql_statement::mysql_statement(mysql_db *db) : db_(db), bindings_(), stmt_(NULL)
        {}

        mysql_statement::mysql_statement(mysql_statement &&other)
        {
            db_ = other.db_;
            bindings_ = other.bindings_;
            stmt_ = other.stmt_;
            other.db_ = NULL;
            other.stmt_ = NULL;
        }

        mysql_statement &mysql_statement::operator=(mysql_statement && other)
        {
            db_ = other.db_;
            bindings_ = other.bindings_;
            stmt_ = other.stmt_;

            other.db_ = NULL;
            other.stmt_ = NULL;

            return *this;
        }

        mysql_statement::~mysql_statement()
        {
            finish();
        }

        void mysql_statement::prepare(const string &sql)
        {
            if (stmt_ != NULL || db_ == NULL || !db_->is_open()) return;

            stmt_ = mysql_stmt_init(db_->db_);

            if (mysql_stmt_prepare(stmt_, sql.c_str(), sql.length()))
                throw database_exception(db_->last_error());
        }

        bool mysql_statement::is_valid() const
        {
            return stmt_ != NULL;
        }

        /**
         * binding methods ensure the dynamic array is sized properly and store the value as a memory pointer
         */

        mysql_statement &mysql_statement::bind(size_t index, int value)
        {
            bindings_.bind(index, value);

            return *this;
        }
        mysql_statement &mysql_statement::bind(size_t index, int64_t value)
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

        mysql_statement &mysql_statement::bind(size_t index, const void *data, size_t size, void (*pFree)(void *))
        {
            bindings_.bind(index, data, size, pFree);

            return *this;
        }

        mysql_statement &mysql_statement::bind_value(size_t index, const sql_value &value)
        {
            value.bind_to(this, index);
            return *this;
        }

        resultset mysql_statement::results()
        {
            bindings_.bind_params(stmt_);

            return resultset(make_shared<mysql_stmt_resultset>(db_, stmt_));
        }

        bool mysql_statement::result()
        {
            bindings_.bind_params(stmt_);

            if (!stmt_ || mysql_stmt_execute(stmt_))
            {
                return false;
            }
            return true;
        }

        int mysql_statement::last_number_of_changes()
        {
            return mysql_stmt_affected_rows(stmt_);
        }

        string mysql_statement::last_error()
        {
            return last_stmt_error(stmt_);
        }

        void mysql_statement::finish()
        {
            bindings_.reset();

            if (stmt_ != NULL)
            {
                mysql_stmt_free_result(stmt_);

                mysql_stmt_close(stmt_);

                stmt_ = NULL;
            }

        }

        void mysql_statement::reset()
        {
            bindings_.reset();

            if (!stmt_ || mysql_stmt_reset(stmt_))
                throw database_exception(last_error());
        }

        long long mysql_statement::last_insert_id()
        {
            if (stmt_ == NULL) return 0;

            return mysql_stmt_insert_id(stmt_);
        }
    }
}

#endif

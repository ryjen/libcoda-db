#include "mysql_statement.h"
#include "mysql_db.h"
#include "mysql_resultset.h"

namespace arg3
{
    namespace db
    {

        // small util method to make a c pointer for a type
        template<typename T>
        void *to_ptr(T value)
        {
            T *ptr = (T *) calloc(1, sizeof(T));

            *ptr = value;

            return ptr;
        }

        string last_stmt_error(MYSQL_STMT *stmt)
        {
            ostringstream buf;

            buf << mysql_stmt_errno(stmt);
            buf << ": " << mysql_stmt_error(stmt);

            return buf.str();
        }

        mysql_statement::mysql_statement(mysql_db *db) : db_(db), bindings_(NULL), stmt_(NULL), bindingSize_(0), refcount_(new unsigned(0))
        {}


        mysql_statement::mysql_statement(const mysql_statement &other) : db_(other.db_), bindings_(other.bindings_),
            stmt_(other.stmt_), bindingSize_(other.bindingSize_), refcount_(other.refcount_)
        {
            (*refcount_)++;
        }
        mysql_statement::mysql_statement(mysql_statement &&other)
        {
            db_ = other.db_;
            bindings_ = other.bindings_;
            stmt_ = other.stmt_;
            bindingSize_ = other.bindingSize_;
            refcount_ = other.refcount_;
            other.db_ = NULL;
            other.bindings_ = NULL;
            other.stmt_ = NULL;
            other.refcount_ = NULL;
        }
        mysql_statement &mysql_statement::operator=(const mysql_statement &other)
        {
            db_ = other.db_;
            bindings_ = other.bindings_;
            stmt_ = other.stmt_;
            bindingSize_ = other.bindingSize_;
            refcount_ = other.refcount_;

            (*refcount_)++;

            return *this;
        }
        mysql_statement &mysql_statement::operator=(mysql_statement && other)
        {
            db_ = other.db_;
            bindings_ = other.bindings_;
            stmt_ = other.stmt_;
            bindingSize_ = other.bindingSize_;
            refcount_ = other.refcount_;

            other.db_ = NULL;
            other.bindings_ = NULL;
            other.stmt_ = NULL;
            other.refcount_ = NULL;

            return *this;
        }

        mysql_statement::~mysql_statement()
        {
            if (refcount_)
            {
                if (*refcount_ > 0)
                {
                    (*refcount_)--;

                    if (*refcount_ != 0)
                        return;
                }

                delete refcount_;
                refcount_ = NULL;
            }

            finish();
        }

        void mysql_statement::prepare(const string &sql)
        {
            if (stmt_ != NULL || !db_->is_open()) return;

            stmt_ = mysql_stmt_init(db_->db_);

            if (mysql_stmt_prepare(stmt_, sql.c_str(), sql.length()))
                throw database_exception(db_->last_error());
        }

        bool mysql_statement::is_valid() const
        {
            return stmt_ != NULL;
        }

        void mysql_statement::reallocate_bindings(size_t index)
        {
            assert(index > 0);

            if (index < bindingSize_)
                return;

            // dynamic array of parameter values
            if (bindings_ == NULL)
            {
                bindings_ = static_cast<MYSQL_BIND *>(calloc(index, sizeof(MYSQL_BIND)));
            }
            else
            {
                bindings_ = static_cast<MYSQL_BIND *>(realloc(bindings_, sizeof(MYSQL_BIND) * (index)));
            }

            // make sure new values are initialized
            for (size_t i = bindingSize_; i < index; i++)
            {
                memset(&bindings_[i], 0, sizeof(MYSQL_BIND));
            }

            bindingSize_ = index;
        }

        /**
         * binding methods ensure the dynamic array is sized properly and store the value as a memory pointer
         */

        mysql_statement &mysql_statement::bind(size_t index, int value)
        {
            reallocate_bindings(index);

            bindings_[index - 1].buffer_type = MYSQL_TYPE_LONG;
            bindings_[index - 1].buffer = to_ptr(value);

            return *this;
        }
        mysql_statement &mysql_statement::bind(size_t index, int64_t value)
        {
            reallocate_bindings(index);
            bindings_[index - 1].buffer_type = MYSQL_TYPE_LONGLONG;
            bindings_[index - 1].buffer = to_ptr(value);

            return *this;
        }
        mysql_statement &mysql_statement::bind(size_t index, double value)
        {
            reallocate_bindings(index);
            bindings_[index - 1].buffer_type = MYSQL_TYPE_DOUBLE;
            bindings_[index - 1].buffer = to_ptr(value);

            return *this;
        }
        mysql_statement &mysql_statement::bind(size_t index, const std::string &value, int len)
        {
            reallocate_bindings(index);
            bindings_[index - 1].buffer_type = MYSQL_TYPE_STRING;
            auto size = len == -1 ? value.size() : len;
            bindings_[index - 1].buffer = strdup(value.c_str());
            bindings_[index - 1].buffer_length = size;

            return *this;
        }
        mysql_statement &mysql_statement::bind(size_t index, const sql_blob &value)
        {
            reallocate_bindings(index);
            bindings_[index - 1].buffer_type = MYSQL_TYPE_BLOB;
            void *ptr = calloc(1, value.size());
            memcpy(ptr, value.ptr(), value.size());
            bindings_[index - 1].buffer = ptr;
            bindings_[index - 1].buffer_length = value.size();

            return *this;
        }
        mysql_statement &mysql_statement::bind(size_t index, const sql_null_t &value)
        {
            reallocate_bindings(index);
            bindings_[index].buffer_type = MYSQL_TYPE_NULL;

            return *this;
        }

        mysql_statement &mysql_statement::bind(size_t index, const void *data, size_t size, void (*pFree)(void *))
        {

            reallocate_bindings(index);
            bindings_[index - 1].buffer_type = MYSQL_TYPE_BLOB;
            void *ptr = calloc(1, size);
            memcpy(ptr, data, size);
            bindings_[index - 1].buffer = ptr;
            bindings_[index - 1].buffer_length = size;

            return *this;
        }

        mysql_statement &mysql_statement::bind_value(size_t index, const sql_value &value)
        {
            value.bind(this, index);
            return *this;
        }

        void mysql_statement::bind_params()
        {
            if (bindings_)
            {
                if (mysql_stmt_bind_param(stmt_, bindings_))
                    throw database_exception(db_->last_error());
            }
        }

        resultset mysql_statement::results()
        {
            bind_params();

            return resultset(make_shared<mysql_stmt_resultset>(db_, stmt_));
        }

        bool mysql_statement::result()
        {
            bind_params();

            if (mysql_stmt_execute(stmt_))
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
            return std::move(last_stmt_error(stmt_));
        }

        void mysql_statement::finish()
        {
            if (bindings_)
            {
                for (size_t i = 0; i < bindingSize_; i++)
                {
                    if (bindings_[i].buffer)
                    {
                        free(bindings_[i].buffer);
                        bindings_[i].buffer = NULL;
                    }
                }
                free(bindings_);
                bindings_ = NULL;
            }

            if (stmt_ != NULL)
            {
                mysql_stmt_free_result(stmt_);

                mysql_stmt_close(stmt_);

                stmt_ = NULL;
            }

        }

        void mysql_statement::reset()
        {
            if (mysql_stmt_reset(stmt_))
                throw database_exception(db_->last_error());
        }
    }
}
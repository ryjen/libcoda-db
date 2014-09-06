#include "config.h"


#ifdef HAVE_LIBMYSQLCLIENT

#include "mysql_binding.h"
#include "exception.h"
#include <memory>
#include <cstdlib>
#include <time.h>
#include <cassert>

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

        extern string last_stmt_error(MYSQL_STMT *stmt);

        mysql_binding::mysql_binding() : value_(NULL), size_(0) {}

        mysql_binding::mysql_binding(size_t size) : value_(NULL), size_(size)
        {
            value_ = (MYSQL_BIND *) calloc(size, sizeof(MYSQL_BIND));
        }

        mysql_binding::mysql_binding(const MYSQL_BIND &value) : value_(NULL), size_(1)
        {
            copy_value(&value, size_);
        }
        mysql_binding::mysql_binding(MYSQL_BIND *values, size_t size) : value_(NULL), size_(size)
        {
            copy_value(values, size);
        }

        mysql_binding::mysql_binding(MYSQL_FIELD *fields, size_t size) : size_(size)
        {
            value_ = (MYSQL_BIND *) calloc(size, sizeof(MYSQL_BIND));

            for (auto i = 0; i < size; i++)
            {
                // get the right field types for mysql_stmt_bind_result()
                switch (fields[i].type)
                {
                case MYSQL_TYPE_INT24:
                    value_[i].buffer_type = MYSQL_TYPE_LONGLONG;
                    break;
                case MYSQL_TYPE_DECIMAL:
                case MYSQL_TYPE_NEWDECIMAL:
                    value_[i].buffer_type = MYSQL_TYPE_DOUBLE;
                    break;
                case MYSQL_TYPE_BIT:
                    value_[i].buffer_type = MYSQL_TYPE_TINY;
                    break;
                case MYSQL_TYPE_YEAR:
                    break;
                case MYSQL_TYPE_VAR_STRING:
                    value_[i].buffer_type = MYSQL_TYPE_STRING;
                    break;
                case MYSQL_TYPE_SET:
                case MYSQL_TYPE_ENUM:
                case MYSQL_TYPE_GEOMETRY:
                    break;
                default:
                    value_[i].buffer_type = fields[i].type;
                    break;
                }
                value_[i].is_null = (my_bool *) calloc(1, sizeof(my_bool));
                value_[i].is_unsigned = 0;
                value_[i].error = 0;
                value_[i].buffer_length = fields[i].length;
                value_[i].length = (size_t *) calloc(1, sizeof(size_t));
                value_[i].buffer = calloc(1, fields[i].length);
            }
        }

        void mysql_binding::clear_value(size_t i)
        {
            assert(i < size_);

            if (value_[i].buffer)
            {
                free(value_[i].buffer);
            }
            if (value_[i].length)
            {
                free(value_[i].length);
            }
            if (value_[i].is_null)
            {
                free(value_[i].is_null);
            }
            if (value_[i].error)
            {
                free(value_[i].error);
            }
            memset(&value_[i], 0, sizeof(value_[i]));
        }

        void mysql_binding::clear_value()
        {
            if (value_)
            {
                for (int i = 0; i < size_; i++)
                {
                    clear_value(i);
                }
                free(value_);
                value_ = NULL;
            }
            size_ = 0;
        }

        void mysql_binding::copy_value(const MYSQL_BIND *others, size_t size)
        {
            clear_value();

            value_ = (MYSQL_BIND *) calloc(size, sizeof(MYSQL_BIND));

            for (int i = 0; i < size; i++)
            {
                const MYSQL_BIND *other = &others[i];
                MYSQL_BIND *value = &value_[i];

                if (other->length)
                {
                    value->length = (unsigned long *) calloc(1, sizeof(unsigned long));
                    memmove(value->length, other->length, sizeof(unsigned long));

                    if (other->buffer)
                    {
                        value->buffer = calloc(1, *other->length);
                        memmove(value->buffer, other->buffer, *other->length);
                    }
                }

                if (other->is_null)
                {
                    value->is_null = (my_bool * ) calloc(1, sizeof(my_bool));
                    memmove(value->is_null, other->is_null, sizeof(my_bool));
                }

                if (other->error)
                {
                    value->error = (my_bool *) calloc(1, sizeof(my_bool));
                    memmove(value->error, other->error, sizeof(my_bool));
                }

                value_->buffer_type = other->buffer_type;
                value_->buffer_length = other->buffer_length;
                value_->is_unsigned = other->is_unsigned;
            }

            size_ = size;
        }

        mysql_binding::mysql_binding(const mysql_binding &other) : value_(NULL)
        {
            copy_value(other.value_, other.size_);
        }
        mysql_binding::mysql_binding(mysql_binding && other)
        {
            value_ = other.value_;
            size_ = other.size_;
            other.value_ = NULL;
            other.size_ = 0;
        }

        mysql_binding &mysql_binding::operator=(const mysql_binding &other)
        {
            copy_value(other.value_, other.size_);
            return *this;
        }

        mysql_binding &mysql_binding::operator=(mysql_binding && other)
        {
            value_ = other.value_;
            size_ = other.size_;
            other.value_ = NULL;
            other.size_ = 0;
            return *this;
        }
        mysql_binding::~mysql_binding()
        {
            clear_value();
        }

        MYSQL_BIND *mysql_binding::get(size_t index) const
        {
            assert(index < size_);

            return &value_[index];
        }

        void mysql_binding::bind_result(MYSQL_STMT *stmt) const
        {
            if (mysql_stmt_bind_result(stmt, value_) != 0)
            {
                throw database_exception(last_stmt_error(stmt));
            }
        }
        sql_blob mysql_binding::to_blob(size_t index) const
        {
            assert(index < size_);

            return to_value(index).to_blob();
        }

        double mysql_binding::to_double(size_t index, const double def) const
        {
            assert(index < size_);

            return to_value(index).to_double(def);
        }
        bool mysql_binding::to_bool(size_t index, const bool def) const
        {
            assert(index < size_);

            return to_value(index).to_bool(def);
        }
        int mysql_binding::to_int(size_t index, const int def) const
        {
            assert(index < size_);

            return to_value(index).to_int(def);
        }

        int64_t mysql_binding::to_int64(size_t index, const int64_t def) const
        {
            assert(index < size_);

            return to_value(index).to_int64(def);
        }

        time_t mysql_binding::to_timestamp(size_t index, const time_t def) const
        {
            assert(index < size_);

            return to_value(index).to_int(def);
        }

        sql_value mysql_binding::to_value(size_t index) const
        {
            assert(index < size_);

            if (value_[index].buffer == NULL)
                return sql_null;

            switch (value_[index].buffer_type)
            {
            case MYSQL_TYPE_TINY:
            case MYSQL_TYPE_SHORT:
            case MYSQL_TYPE_LONG:
            case MYSQL_TYPE_INT24:
            {
                int *p = static_cast<int *>(value_[index].buffer);
                if (p == NULL) return sql_null;
                return *p;
            }
            case MYSQL_TYPE_LONGLONG:
            {
                int64_t *p = static_cast<int64_t *>(value_[index].buffer);

                if (p == NULL) return sql_null;

                return *p;
            }
            case MYSQL_TYPE_NULL:
                return sql_null;
            case MYSQL_TYPE_TIME:
            case MYSQL_TYPE_DATE:
            case MYSQL_TYPE_TIMESTAMP:
            case MYSQL_TYPE_DATETIME:
            {
                struct tm *tp;

                if ((tp = getdate(static_cast<char *>(value_[index].buffer))))
                {
                    return (long long) mktime(tp);
                }
                else
                {
                    int *p = static_cast<int *>(value_[index].buffer);
                    if (p == NULL) return sql_null;
                    return *p;
                }
            }
            default:
            {
                char *p = static_cast<char *>(value_[index].buffer);

                if (p == NULL) return sql_null;

                return p;
            }
            case MYSQL_TYPE_FLOAT:
            case MYSQL_TYPE_DOUBLE:
            {
                double *p = static_cast<double *>(value_[index].buffer);

                if (p == NULL) return sql_null;

                return *p;
            }
            case MYSQL_TYPE_BLOB:
            {
                if (value_[index].length)
                {
                    void *buf = calloc(1, *value_[index].length);
                    memmove(buf, value_[index].buffer, *value_[index].length);

                    return sql_blob(buf, *value_[index].length, free);
                }

                return sql_blob(NULL, 0, NULL);
            }
            }
        }

        int mysql_binding::type(size_t index) const
        {
            assert(index < size_);

            return value_[index].buffer_type;
        }

        string mysql_binding::to_string(size_t index) const
        {
            assert(index < size_);

            return to_value(index).to_string();
        }

        void mysql_binding::reallocate_value(size_t index)
        {
            assert(index > 0);

            if (index <= size_)
            {
                clear_value(index - 1);
                return;
            }

            // dynamic array of parameter values
            if (value_ == NULL)
            {
                value_ = static_cast<MYSQL_BIND *>(calloc(index, sizeof(MYSQL_BIND)));
            }
            else
            {
                value_ = static_cast<MYSQL_BIND *>(realloc(value_, sizeof(MYSQL_BIND) * (index)));

                // make sure new values are initialized
                for (size_t i = size_; i < index; i++)
                {
                    memset(&value_[i], 0, sizeof(MYSQL_BIND));
                }
            }

            size_ = index;
        }

        /**
         * binding methods ensure the dynamic array is sized properly and store the value as a memory pointer
         */

        mysql_binding &mysql_binding::bind(size_t index, int value)
        {
            reallocate_value(index);

            value_[index - 1].buffer_type = MYSQL_TYPE_LONG;
            value_[index - 1].buffer = to_ptr(value);

            return *this;
        }
        mysql_binding &mysql_binding::bind(size_t index, int64_t value)
        {
            reallocate_value(index);
            value_[index - 1].buffer_type = MYSQL_TYPE_LONGLONG;
            value_[index - 1].buffer = to_ptr(value);

            return *this;
        }
        mysql_binding &mysql_binding::bind(size_t index, double value)
        {
            reallocate_value(index);
            value_[index - 1].buffer_type = MYSQL_TYPE_DOUBLE;
            value_[index - 1].buffer = to_ptr(value);

            return *this;
        }
        mysql_binding &mysql_binding::bind(size_t index, const std::string &value, int len)
        {
            reallocate_value(index);
            value_[index - 1].buffer_type = MYSQL_TYPE_STRING;
            auto size = len == -1 ? value.size() : len;
            value_[index - 1].buffer = strdup(value.c_str());
            value_[index - 1].buffer_length = size;

            return *this;
        }
        mysql_binding &mysql_binding::bind(size_t index, const sql_blob &value)
        {
            reallocate_value(index);
            value_[index - 1].buffer_type = MYSQL_TYPE_BLOB;
            void *ptr = calloc(1, value.size());
            memcpy(ptr, value.ptr(), value.size());
            value_[index - 1].buffer = ptr;
            value_[index - 1].buffer_length = value.size();

            return *this;
        }
        mysql_binding &mysql_binding::bind(size_t index, const sql_null_type &value)
        {
            reallocate_value(index);
            value_[index - 1].buffer_type = MYSQL_TYPE_NULL;

            return *this;
        }

        mysql_binding &mysql_binding::bind(size_t index, const void *data, size_t size, void (*pFree)(void *))
        {

            reallocate_value(index);
            value_[index - 1].buffer_type = MYSQL_TYPE_BLOB;
            void *ptr = calloc(1, size);
            memcpy(ptr, data, size);
            value_[index - 1].buffer = ptr;
            value_[index - 1].buffer_length = size;

            return *this;
        }

        mysql_binding &mysql_binding::bind_value(size_t index, const sql_value &value)
        {
            value.bind_to(this, index);
            return *this;
        }

        void mysql_binding::bind_params(MYSQL_STMT *stmt) const
        {
            if (mysql_stmt_bind_param(stmt, value_))
                throw database_exception(last_stmt_error(stmt));

        }

        size_t mysql_binding::size() const
        {
            return size_;
        }

        void mysql_binding::reset()
        {
            clear_value();
        }

    }
}

#endif

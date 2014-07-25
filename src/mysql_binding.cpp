#include "config.h"


#ifdef HAVE_LIBMYSQLCLIENT

#include "mysql_binding.h"
#include "exception.h"
#include <memory>
#include <cstdlib>

namespace arg3
{
    namespace db
    {


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

        void mysql_binding::clear_value()
        {
            if (value_)
            {
                for (int i = 0; i < size_; i++)
                {
                    if (value_[i].buffer)
                    {
                        free(value_[i].buffer);
                        value_[i].buffer = NULL;
                    }
                    if (value_[i].length)
                    {
                        free(value_[i].length);
                        value_[i].length = NULL;
                    }
                    if (value_[i].is_null)
                    {
                        free(value_[i].is_null);
                        value_[i].is_null = NULL;
                    }
                    if (value_[i].error)
                    {
                        free(value_[i].error);
                        value_[i].error = NULL;
                    }
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

        shared_ptr<mysql_binding> mysql_binding::get(size_t index) const
        {
            assert(index < size_);

            return make_shared<mysql_binding>(value_[index]);
        }

        void mysql_binding::bind_result(MYSQL_STMT *stmt)
        {
            if (mysql_stmt_bind_result(stmt, value_) != 0)
            {
                throw database_exception(last_stmt_error(stmt));
            }
        }
        sql_blob mysql_binding::to_blob(size_t index) const
        {
            assert(index < size_);

            if (value_[index].buffer && value_[index].length)
            {
                void *buf = calloc(1, *value_[index].length);
                memmove(buf, value_[index].buffer, *value_[index].length);

                return sql_blob(buf, *value_[index].length, free);
            }

            return sql_blob(NULL, 0, NULL);
        }

        double mysql_binding::to_double(size_t index) const
        {
            assert(index < size_);

            double *value = static_cast<double *>(value_[index].buffer);

            return value == NULL ? 0 : *value;
        }
        bool mysql_binding::to_bool(size_t index) const
        {
            assert(index < size_);

            int *value = static_cast<int *>(value_[index].buffer);

            return value != NULL && *value;
        }
        int mysql_binding::to_int(size_t index) const
        {
            assert(index < size_);

            int *value = static_cast<int *>(value_[index].buffer);

            return value == NULL ? 0 : *value;
        }

        int64_t mysql_binding::to_int64(size_t index) const
        {
            assert(index < size_);

            int64_t *value = static_cast<int64_t *>(value_[index].buffer);

            return value == NULL ? 0 : *value;
        }

        sql_value mysql_binding::to_value(size_t index) const
        {
            assert(index < size_);

            switch (value_[index].buffer_type)
            {
            case MYSQL_TYPE_TINY:
            case MYSQL_TYPE_SHORT:
            case MYSQL_TYPE_LONG:
            case MYSQL_TYPE_INT24:
            case MYSQL_TYPE_LONGLONG:
                return sql_value(to_int64(index));
            default:
                return sql_value(to_string(index));
            case MYSQL_TYPE_FLOAT:
            case MYSQL_TYPE_DOUBLE:
                return sql_value(to_double(index));
            case MYSQL_TYPE_BLOB:
                return sql_value(to_blob(index));
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

            if (value_[index].is_null && *value_[index].is_null)
                return string();

            auto textValue = static_cast<char *>(value_[index].buffer);

            if (textValue == NULL)
                return string();

            return textValue;
        }
    }
}

#endif

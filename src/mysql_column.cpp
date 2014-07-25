#include "config.h"

#ifdef HAVE_LIBMYSQLCLIENT

#include <string>
#include "mysql_column.h"

namespace arg3
{
    namespace db
    {

        mysql_column::mysql_column(MYSQL_RES *res, MYSQL_ROW pValue, size_t index) : value_(pValue), res_(res), index_(index)
        {
            assert(value_ != NULL);
            assert(res_ != NULL);
        }

        mysql_column::mysql_column(const mysql_column &other) : value_(other.value_), res_(other.res_), index_(other.index_) {}

        mysql_column::mysql_column(mysql_column &&other) : value_(other.value_), res_(other.res_), index_(other.index_)
        {
            other.value_ = NULL;
            other.res_ = NULL;
        }

        mysql_column::~mysql_column() {}

        mysql_column &mysql_column::operator=(const mysql_column &other)
        {
            value_ = other.value_;
            index_ = other.index_;
            res_ = other.res_;

            return *this;
        }

        mysql_column &mysql_column::operator=(mysql_column && other)
        {
            value_ = other.value_;
            index_ = other.index_;
            res_ = other.res_;

            other.value_ = NULL;
            other.res_ = NULL;

            return *this;
        }

        bool mysql_column::is_valid() const
        {
            return value_ != NULL;
        }

        sql_blob mysql_column::to_blob() const
        {
            assert(res_ != NULL && value_ != NULL);

            auto lengths = mysql_fetch_lengths(res_);

            void *buf = calloc(1, lengths[index_]);
            memmove(buf, value_[index_], lengths[index_]);

            return sql_blob(buf, lengths[index_], free);
        }

        double mysql_column::to_double() const
        {
            return std::stod(value_[index_]);
        }
        bool mysql_column::to_bool() const
        {
            return std::stoi(value_[index_]) != 0;
        }
        int mysql_column::to_int() const
        {
            return std::stoi(value_[index_]);
        }

        int64_t mysql_column::to_int64() const
        {
            return std::stoll(value_[index_]);
        }

        sql_value mysql_column::to_value() const
        {
            auto field = mysql_fetch_field_direct(res_, index_);

            switch (field->type)
            {
            case MYSQL_TYPE_TINY:
            case MYSQL_TYPE_SHORT:
            case MYSQL_TYPE_LONG:
            case MYSQL_TYPE_INT24:
            case MYSQL_TYPE_LONGLONG:
                return sql_value(to_int64());
            default:
                return sql_value(to_string());
            case MYSQL_TYPE_FLOAT:
            case MYSQL_TYPE_DOUBLE:
                return sql_value(to_double());
            case MYSQL_TYPE_BLOB:
                return sql_value(to_blob());
            }
        }

        int mysql_column::type() const
        {
            assert(res_ != NULL);

            auto field = mysql_fetch_field_direct(res_, index_);

            return field->type;
        }

        string mysql_column::to_string() const
        {
            assert(value_ != NULL);

            auto textValue = value_[index_];

            if (textValue == NULL)
                return string();

            return textValue;
        }


        /* statement version */

        void mysql_stmt_column::copy_value(const MYSQL_BIND &other)
        {
            memset(&value_, 0, sizeof(MYSQL_BIND));

            if (other.length)
            {
                value_.length = (unsigned long *) calloc(1, sizeof(unsigned long));
                memmove(value_.length, other.length, sizeof(unsigned long));

                if (other.buffer)
                {
                    value_.buffer = calloc(1, *other.length);
                    memmove(value_.buffer, other.buffer, *other.length);
                }
            }

            if (other.is_null)
            {
                value_.is_null = (my_bool * ) calloc(1, sizeof(my_bool));
                memmove(value_.is_null, other.is_null, sizeof(my_bool));
            }

            if (other.error)
            {
                value_.error = (my_bool *) calloc(1, sizeof(my_bool));
                memmove(value_.error, other.error, sizeof(my_bool));
            }

            value_.buffer_type = other.buffer_type;
            value_.buffer_length = other.buffer_length;
            value_.is_unsigned = other.is_unsigned;
        }
        mysql_stmt_column::mysql_stmt_column(const MYSQL_BIND &pValue)
        {
            copy_value(pValue);
        }

        mysql_stmt_column::mysql_stmt_column(const mysql_stmt_column &other)
        {
            copy_value(other.value_);
        }

        mysql_stmt_column::mysql_stmt_column(mysql_stmt_column &&other) : value_(other.value_)
        {
            other.value_.buffer = NULL;
        }

        mysql_stmt_column::~mysql_stmt_column()
        {
            if (value_.length)
            {
                free(value_.length);
                value_.length = 0;
            }
            if (value_.buffer)
            {
                free(value_.buffer);
                value_.buffer = 0;
            }
            if (value_.is_null)
            {
                free(value_.is_null);
                value_.is_null = 0;
            }
            if (value_.error)
            {
                free(value_.error);
                value_.error = 0;
            }
        }

        mysql_stmt_column &mysql_stmt_column::operator=(const mysql_stmt_column &other)
        {
            copy_value(other.value_);

            return *this;
        }

        mysql_stmt_column &mysql_stmt_column::operator=(mysql_stmt_column && other)
        {
            value_ = other.value_;
            other.value_.buffer = NULL;
            return *this;
        }

        bool mysql_stmt_column::is_valid() const
        {
            return value_.buffer != NULL;
        }

        sql_blob mysql_stmt_column::to_blob() const
        {
            if (value_.length)
            {
                void *buf = calloc(1, *value_.length);
                memmove(buf, value_.buffer, *value_.length);

                return sql_blob(buf, *value_.length, free);
            }

            return sql_blob(NULL, 0, NULL);
        }

        double mysql_stmt_column::to_double() const
        {
            double *value = static_cast<double *>(value_.buffer);

            return value == NULL ? 0 : *value;
        }
        bool mysql_stmt_column::to_bool() const
        {
            int *value = static_cast<int *>(value_.buffer);

            return value != NULL && *value;
        }
        int mysql_stmt_column::to_int() const
        {
            int *value = static_cast<int *>(value_.buffer);

            return value == NULL ? 0 : *value;
        }

        int64_t mysql_stmt_column::to_int64() const
        {
            int64_t *value = static_cast<int64_t *>(value_.buffer);

            return value == NULL ? 0 : *value;
        }

        sql_value mysql_stmt_column::to_value() const
        {
            switch (value_.buffer_type)
            {
            case MYSQL_TYPE_TINY:
            case MYSQL_TYPE_SHORT:
            case MYSQL_TYPE_LONG:
            case MYSQL_TYPE_INT24:
            case MYSQL_TYPE_LONGLONG:
                return sql_value(to_int64());
            default:
                return sql_value(to_string());
            case MYSQL_TYPE_FLOAT:
            case MYSQL_TYPE_DOUBLE:
                return sql_value(to_double());
            case MYSQL_TYPE_BLOB:
                return sql_value(to_blob());
            }
        }

        int mysql_stmt_column::type() const
        {
            return value_.buffer_type;
        }

        string mysql_stmt_column::to_string() const
        {
            if (value_.is_null && *value_.is_null)
                return string();

            auto textValue = static_cast<char *>(value_.buffer);

            if (textValue == NULL)
                return string();

            return textValue;
        }
    }
}

#endif

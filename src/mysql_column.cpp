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
            memcpy(buf, value_[index_], lengths[index_]);

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

        mysql_stmt_column::mysql_stmt_column(MYSQL_BIND *pValue) : value_(pValue)
        {
            assert(value_ != NULL);
        }

        mysql_stmt_column::mysql_stmt_column(const mysql_stmt_column &other) : value_(other.value_) {}

        mysql_stmt_column::mysql_stmt_column(mysql_stmt_column &&other) : value_(other.value_)
        {
            other.value_ = NULL;
        }

        mysql_stmt_column::~mysql_stmt_column() {}

        mysql_stmt_column &mysql_stmt_column::operator=(const mysql_stmt_column &other)
        {
            value_ = other.value_;

            return *this;
        }

        mysql_stmt_column &mysql_stmt_column::operator=(mysql_stmt_column && other)
        {
            value_ = other.value_;
            other.value_ = NULL;
            return *this;
        }

        bool mysql_stmt_column::is_valid() const
        {
            return value_ != NULL;
        }

        sql_blob mysql_stmt_column::to_blob() const
        {
            if (value_->length)
            {
                void *buf = calloc(1, *value_->length);
                memcpy(buf, value_->buffer, *value_->length);

                return sql_blob(buf, *value_->length, free);
            }

            return sql_blob(NULL, 0, NULL);
        }

        double mysql_stmt_column::to_double() const
        {
            assert(value_->buffer != NULL);
            double *value = static_cast<double *>(value_->buffer);

            return value == NULL ? 0 : *value;
        }
        bool mysql_stmt_column::to_bool() const
        {
            assert(value_->buffer != NULL);

            int *value = static_cast<int *>(value_->buffer);

            return value != NULL && *value;
        }
        int mysql_stmt_column::to_int() const
        {
            assert(value_->buffer != NULL);
            int *value = static_cast<int *>(value_->buffer);

            return value == NULL ? 0 : *value;
        }

        int64_t mysql_stmt_column::to_int64() const
        {
            assert(value_->buffer != NULL);
            int64_t *value = static_cast<int64_t *>(value_->buffer);

            return value == NULL ? 0 : *value;
        }

        sql_value mysql_stmt_column::to_value() const
        {
            switch (value_->buffer_type)
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
            return value_->buffer_type;
        }

        string mysql_stmt_column::to_string() const
        {
            if (value_->is_null && *value_->is_null)
                return string();

            assert(value_->buffer != NULL);

            auto textValue = static_cast<char *>(value_->buffer);

            if (textValue == NULL)
                return string();

            return textValue;
        }
    }
}

#endif

#include "config.h"

#ifdef HAVE_LIBMYSQLCLIENT

#include <string>
#include "mysql_column.h"
#include "mysql_binding.h"

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

        mysql_stmt_column::mysql_stmt_column(shared_ptr<mysql_binding> value) : value_(value)
        {
        }

        mysql_stmt_column::mysql_stmt_column(const mysql_stmt_column &other) : value_(other.value_)
        {
        }

        mysql_stmt_column::mysql_stmt_column(mysql_stmt_column &&other) : value_(other.value_)
        {
            other.value_ = nullptr;
        }

        mysql_stmt_column::~mysql_stmt_column()
        {

        }

        mysql_stmt_column &mysql_stmt_column::operator=(const mysql_stmt_column &other)
        {
            value_ = other.value_;

            return *this;
        }

        mysql_stmt_column &mysql_stmt_column::operator=(mysql_stmt_column && other)
        {
            value_ = other.value_;
            other.value_ = nullptr;
            return *this;
        }

        bool mysql_stmt_column::is_valid() const
        {
            return value_ != nullptr;
        }

        sql_blob mysql_stmt_column::to_blob() const
        {
            assert(value_ != nullptr);

            return value_->to_blob(0);
        }

        double mysql_stmt_column::to_double() const
        {
            assert(value_ != nullptr);

            return value_->to_double(0);
        }
        bool mysql_stmt_column::to_bool() const
        {
            assert(value_ != nullptr);

            return value_->to_bool(0);
        }
        int mysql_stmt_column::to_int() const
        {
            assert(value_ != nullptr);

            return value_->to_int(0);
        }

        int64_t mysql_stmt_column::to_int64() const
        {
            assert(value_ != nullptr);

            return value_->to_int64(0);
        }

        sql_value mysql_stmt_column::to_value() const
        {
            assert(value_ != nullptr);

            return value_->to_value(0);
        }

        int mysql_stmt_column::type() const
        {
            assert(value_ != nullptr);

            return value_->type(0);
        }

        string mysql_stmt_column::to_string() const
        {
            assert(value_ != nullptr);

            return value_->to_string(0);
        }
    }
}

#endif

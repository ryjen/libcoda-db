#include "config.h"

#ifdef HAVE_LIBMYSQLCLIENT

#include <string>
#include "mysql_column.h"
#include "mysql_binding.h"

namespace arg3
{
    namespace db
    {

        mysql_column::mysql_column(shared_ptr<MYSQL_RES> res, MYSQL_ROW pValue, size_t index) : value_(pValue), res_(res), index_(index)
        {
            assert(value_ != NULL);
            assert(res_ != nullptr);
        }

        mysql_column::mysql_column(const mysql_column &other) : value_(other.value_), res_(other.res_), index_(other.index_) {}

        mysql_column::mysql_column(mysql_column &&other) : value_(other.value_), res_(other.res_), index_(other.index_)
        {
            other.value_ = NULL;
            other.res_ = nullptr;
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
            other.res_ = nullptr;

            return *this;
        }

        bool mysql_column::is_valid() const
        {
            return value_ != NULL;
        }

        sql_blob mysql_column::to_blob() const
        {
            assert(res_ != nullptr && value_ != NULL);

            auto lengths = mysql_fetch_lengths(res_.get());

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
            auto field = mysql_fetch_field_direct(res_.get(), index_);

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
            assert(res_ != nullptr);

            auto field = mysql_fetch_field_direct(res_.get(), index_);

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

        string mysql_column::name() const
        {
            auto field = mysql_fetch_field_direct(res_.get(), index_);

            return field->name;
        }


        /* statement version */

        mysql_stmt_column::mysql_stmt_column(const string &name, shared_ptr<mysql_binding> bindings, size_t position) : name_(name), value_(bindings), position_(position)
        {
        }

        mysql_stmt_column::mysql_stmt_column(const mysql_stmt_column &other) : name_(other.name_), value_(other.value_), position_(other.position_)
        {
        }

        mysql_stmt_column::mysql_stmt_column(mysql_stmt_column &&other) : name_(std::move(other.name_)), value_(other.value_), position_(other.position_)
        {
            other.value_ = nullptr;
        }

        mysql_stmt_column::~mysql_stmt_column()
        {

        }

        mysql_stmt_column &mysql_stmt_column::operator=(const mysql_stmt_column &other)
        {
            value_ = other.value_;
            name_ = other.name_;
            position_ = other.position_;

            return *this;
        }

        mysql_stmt_column &mysql_stmt_column::operator=(mysql_stmt_column && other)
        {
            name_ = std::move(other.name_);
            value_ = other.value_;
            position_ = other.position_;
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

            return value_->to_blob(position_);
        }

        double mysql_stmt_column::to_double() const
        {
            assert(value_ != nullptr);

            return value_->to_double(position_);
        }
        bool mysql_stmt_column::to_bool() const
        {
            assert(value_ != nullptr);

            return value_->to_bool(position_);
        }
        int mysql_stmt_column::to_int() const
        {
            assert(value_ != nullptr);

            return value_->to_int(position_);
        }

        int64_t mysql_stmt_column::to_int64() const
        {
            assert(value_ != nullptr);

            return value_->to_int64(position_);
        }

        sql_value mysql_stmt_column::to_value() const
        {
            assert(value_ != nullptr);

            return value_->to_value(position_);
        }

        int mysql_stmt_column::type() const
        {
            assert(value_ != nullptr);

            return value_->type(position_);
        }

        string mysql_stmt_column::to_string() const
        {
            assert(value_ != nullptr);

            return value_->to_string(position_);
        }

        string mysql_stmt_column::name() const
        {
            return name_;
        }


        /* cached version */


        mysql_cached_column::mysql_cached_column(const string &name, mysql_binding &bindings, size_t position) : name_(name),
            value_(bindings.to_value(position)), type_(bindings.type(position))
        {
        }

        mysql_cached_column::mysql_cached_column(shared_ptr<MYSQL_RES> res, MYSQL_ROW pValue, size_t index)
        {
            assert(res != nullptr);

            auto field = mysql_fetch_field_direct(res.get(), index);

            type_ = field->type;

            name_ = field->name;

            switch (field->type)
            {
            case MYSQL_TYPE_TINY:
            case MYSQL_TYPE_SHORT:
            case MYSQL_TYPE_LONG:
            case MYSQL_TYPE_INT24:
            case MYSQL_TYPE_LONGLONG:
                value_ = std::stoll(pValue[index]);
                break;
            default:
            {
                auto textValue = pValue[index];

                if (textValue == NULL)
                    value_ = sql_null;
                else
                    value_ = textValue;
                break;
            }
            case MYSQL_TYPE_FLOAT:
            case MYSQL_TYPE_DOUBLE:
                value_ = std::stod(pValue[index]);
                break;
            case MYSQL_TYPE_BLOB:
            {

                auto lengths = mysql_fetch_lengths(res.get());

                void *buf = calloc(1, lengths[index]);
                memmove(buf, pValue[index], lengths[index]);

                value_ = sql_blob(buf, lengths[index], free);
                break;
            }
            }
        }

        bool mysql_cached_column::is_valid() const
        {
            return true;
        }

        sql_blob mysql_cached_column::to_blob() const
        {
            return value_;
        }

        double mysql_cached_column::to_double() const
        {
            return value_;
        }
        bool mysql_cached_column::to_bool() const
        {
            return value_;
        }
        int mysql_cached_column::to_int() const
        {
            return value_;
        }

        int64_t mysql_cached_column::to_int64() const
        {
            return value_;
        }

        sql_value mysql_cached_column::to_value() const
        {
            return value_;
        }

        int mysql_cached_column::type() const
        {
            return type_;
        }

        string mysql_cached_column::to_string() const
        {
            return value_;
        }

        string mysql_cached_column::name() const
        {
            return name_;
        }
    }
}

#endif

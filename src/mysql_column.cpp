#include "config.h"

#ifdef HAVE_LIBMYSQLCLIENT

#include <string>
#include <time.h>
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

            if (value_[index_] == NULL)
            {
                return sql_blob(NULL, 0, NULL);
            }
            auto lengths = mysql_fetch_lengths(res_.get());

            void *buf = calloc(1, lengths[index_]);
            memmove(buf, value_[index_], lengths[index_]);

            return sql_blob(buf, lengths[index_], free);
        }

        double mysql_column::to_double() const
        {
            if (value_[index_] == NULL) return DOUBLE_DEFAULT;

            try
            {
                return std::stod(value_[index_]);
            }
            catch ( const std::exception &e)
            {
                return DOUBLE_DEFAULT;
            }
        }
        bool mysql_column::to_bool() const
        {
            if (value_[index_] == NULL) return BOOL_DEFAULT;
            try
            {
                if (!strcasecmp(value_[index_], "true") || !strcasecmp(value_[index_], "yes"))
                    return true;
                if (!strcasecmp(value_[index_], "false") || !strcasecmp(value_[index_], "no"))
                    return false;

                return std::stoi(value_[index_]) != 0;
            }
            catch ( const std::exception &e)
            {
                return BOOL_DEFAULT;
            }
        }
        int mysql_column::to_int() const
        {
            if (value_[index_] == NULL) return INT_DEFAULT;
            try
            {
                return std::stoi(value_[index_]);
            }
            catch (const std::exception &e)
            {
                return INT_DEFAULT;
            }
        }

        long long mysql_column::to_llong() const
        {
            if (value_[index_] == NULL) return INT_DEFAULT;

            try
            {
                return std::stoll(value_[index_]);
            }
            catch (const std::exception &e)
            {
                return INT_DEFAULT;
            }
        }

        time_t mysql_column::to_timestamp() const
        {
            if (value_[index_] == NULL) return INT_DEFAULT;

            struct tm *tp;

            if ((tp = getdate(value_[index_])))
            {
                return mktime(tp);
            }

            return INT_DEFAULT;
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
                return to_llong();
            case MYSQL_TYPE_DATE:
            case MYSQL_TYPE_DATETIME:
            case MYSQL_TYPE_TIMESTAMP:
            case MYSQL_TYPE_TIME:
                return (long long) to_timestamp();
            default:
                return to_string();
            case MYSQL_TYPE_FLOAT:
            case MYSQL_TYPE_DOUBLE:
                return to_double();
            case MYSQL_TYPE_BLOB:
                return to_blob();
            case MYSQL_TYPE_NULL:
                return sql_null;
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

            if (value_[index_] == NULL)
                return string();

            return value_[index_];
        }

        string mysql_column::name() const
        {
            auto field = mysql_fetch_field_direct(res_.get(), index_);

            if (field == NULL || field->name == NULL)
                return string();

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

            return value_->to_double(position_, DOUBLE_DEFAULT);
        }
        bool mysql_stmt_column::to_bool() const
        {
            assert(value_ != nullptr);

            return value_->to_bool(position_, BOOL_DEFAULT);
        }
        int mysql_stmt_column::to_int() const
        {
            assert(value_ != nullptr);

            return value_->to_int(position_, INT_DEFAULT);
        }

        long long mysql_stmt_column::to_llong() const
        {
            assert(value_ != nullptr);

            return value_->to_llong(position_, INT_DEFAULT);
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

            if (field->name)
                name_ = field->name;

            switch (field->type)
            {
            case MYSQL_TYPE_TINY:
            case MYSQL_TYPE_SHORT:
            case MYSQL_TYPE_LONG:
            case MYSQL_TYPE_INT24:
            case MYSQL_TYPE_LONGLONG:
                if (pValue[index])
                    value_ = std::stoll(pValue[index]);
                else
                    value_ = sql_null;
                break;
            case MYSQL_TYPE_NULL:
                value_ = sql_null;
                break;
            case MYSQL_TYPE_TIME:
            case MYSQL_TYPE_DATE:
            case MYSQL_TYPE_DATETIME:
            case MYSQL_TYPE_TIMESTAMP:
            {
                struct tm *tp;

                if (pValue[index] && (tp = getdate(pValue[index])))
                {
                    long long epoch = mktime(tp);
                    value_ = epoch;
                }
                else
                {
                    value_ = sql_null;
                }
                break;
            }
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
                if (pValue[index])
                    value_ = std::stod(pValue[index]);
                else
                    value_ = sql_null;
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

        mysql_cached_column::mysql_cached_column(const mysql_cached_column &other) : name_(other.name_), value_(other.value_), type_(other.type_)
        {}

        mysql_cached_column::mysql_cached_column(mysql_cached_column &&other) : name_(std::move(other.name_)), value_(std::move(other.value_)), type_(other.type_)
        {
            other.value_ = nullptr;
        }
        mysql_cached_column::~mysql_cached_column() {}

        mysql_cached_column &mysql_cached_column::operator=(const mysql_cached_column &other)
        {
            name_ = other.name_;
            value_ = other.value_;
            type_ = other.type_;

            return *this;
        }
        mysql_cached_column &mysql_cached_column::operator=(mysql_cached_column && other)
        {
            name_ = std::move(other.name_);
            value_ = std::move(other.value_);
            type_ = other.type_;

            return *this;
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
            return value_.to_double(DOUBLE_DEFAULT);
        }
        bool mysql_cached_column::to_bool() const
        {
            return value_.to_bool(BOOL_DEFAULT);
        }
        int mysql_cached_column::to_int() const
        {
            return value_.to_int(INT_DEFAULT);
        }

        long long mysql_cached_column::to_llong() const
        {
            return value_.to_llong(INT_DEFAULT);
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

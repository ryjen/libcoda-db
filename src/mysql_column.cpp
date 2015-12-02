#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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
            if(value_ == nullptr) {
                throw database_exception("no mysql value provided for column");
            }
            if(res_ == nullptr) {
                throw database_exception("no mysql result provided for column");
            }
        }

        mysql_column::mysql_column(const mysql_column &other) : value_(other.value_), res_(other.res_), index_(other.index_)
        {
        }

        mysql_column::mysql_column(mysql_column &&other) : value_(other.value_), res_(other.res_), index_(other.index_)
        {
            other.value_ = NULL;
            other.res_ = nullptr;
        }

        mysql_column::~mysql_column()
        {
        }

        mysql_column &mysql_column::operator=(const mysql_column &other)
        {
            value_ = other.value_;
            index_ = other.index_;
            res_ = other.res_;

            return *this;
        }

        mysql_column &mysql_column::operator=(mysql_column &&other)
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
            return value_ != nullptr;
        }

        sql_value mysql_column::to_value() const
        {
            auto field = mysql_fetch_field_direct(res_.get(), index_);

            if (value_[index_] == nullptr || field == nullptr) {
                throw no_such_column_exception();
            }

            switch (field->type) {
                case MYSQL_TYPE_BIT:
                case MYSQL_TYPE_TINY:
                case MYSQL_TYPE_SHORT:
                case MYSQL_TYPE_LONG: {
                    try {
                        return std::stoi(value_[index_]);
                    } catch (const std::exception &e) {
                        return sql_value();
                    }
                }
                case MYSQL_TYPE_INT24:
                case MYSQL_TYPE_LONGLONG: {
                    try {
                        return std::stoll(value_[index_]);
                    } catch (const std::exception &e) {
                        return sql_value();
                    }
                }
                case MYSQL_TYPE_DECIMAL:
                case MYSQL_TYPE_VARCHAR:
                case MYSQL_TYPE_VAR_STRING:
                case MYSQL_TYPE_NEWDECIMAL:
                case MYSQL_TYPE_GEOMETRY:
                case MYSQL_TYPE_ENUM:
                case MYSQL_TYPE_SET:
                case MYSQL_TYPE_STRING:
                default:
                    return value_[index_];


                case MYSQL_TYPE_NEWDATE:
                case MYSQL_TYPE_DATE:
                case MYSQL_TYPE_DATETIME:
                case MYSQL_TYPE_TIMESTAMP:
                case MYSQL_TYPE_YEAR:
                case MYSQL_TYPE_TIME: {
                    struct tm *tp;

                    if ((tp = getdate(value_[index_]))) {
                        return mktime(tp);
                    }

                    return sql_value();
                }
                case MYSQL_TYPE_FLOAT:
                case MYSQL_TYPE_DOUBLE: {
                    try {
                        return std::stod(value_[index_]);
                    } catch (const std::exception &e) {
                        return sql_value();
                    }
                }
                case MYSQL_TYPE_TINY_BLOB:
                case MYSQL_TYPE_MEDIUM_BLOB:
                case MYSQL_TYPE_LONG_BLOB:
                case MYSQL_TYPE_BLOB: {
                    auto lengths = mysql_fetch_lengths(res_.get());
                    return sql_blob(value_[index_], lengths[index_]);
                }
                case MYSQL_TYPE_NULL:
                    return sql_null;
            }
        }

        int mysql_column::sql_type() const
        {
            auto field = mysql_fetch_field_direct(res_.get(), index_);

            if (field == nullptr) {
                return MYSQL_TYPE_NULL;
            }

            return field->type;
        }

        string mysql_column::name() const
        {
            auto field = mysql_fetch_field_direct(res_.get(), index_);

            if (field == NULL || field->name == NULL) {
                return string();
            }

            return field->name;
        }


        /* statement version */

        mysql_stmt_column::mysql_stmt_column(const string &name, shared_ptr<mysql_binding> bindings, size_t position)
            : name_(name), value_(bindings), position_(position)
        {
            if (bindings == nullptr) {
                throw database_exception("no mysql bindings provided for column");
            }
        }

        mysql_stmt_column::mysql_stmt_column(const mysql_stmt_column &other) : name_(other.name_), value_(other.value_), position_(other.position_)
        {
        }

        mysql_stmt_column::mysql_stmt_column(mysql_stmt_column &&other)
            : name_(std::move(other.name_)), value_(other.value_), position_(other.position_)
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

        mysql_stmt_column &mysql_stmt_column::operator=(mysql_stmt_column &&other)
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

        sql_value mysql_stmt_column::to_value() const
        {
            assert(value_ != nullptr);

            return value_->to_value(position_);
        }

        int mysql_stmt_column::sql_type() const
        {
            assert(value_ != nullptr);

            return value_->sql_type(position_);
        }

        string mysql_stmt_column::name() const
        {
            return name_;
        }


        /* cached version */


        mysql_cached_column::mysql_cached_column(const string &name, mysql_binding &bindings, size_t position)
            : name_(name), value_(bindings.to_value(position)), type_(bindings.sql_type(position))
        {
        }

        mysql_cached_column::mysql_cached_column(shared_ptr<MYSQL_RES> res, MYSQL_ROW pValue, size_t index)
        {
            if(res == nullptr) {
                throw database_exception("no mysql result provided for column");
            }

            set_value(res, pValue, index);
        }

        void mysql_cached_column::set_value(shared_ptr<MYSQL_RES> res, MYSQL_ROW pValue, size_t index)
        {
            auto field = mysql_fetch_field_direct(res.get(), index);

            if (field == nullptr) {
                throw no_such_column_exception();
            }

            type_ = field->type;

            if (field->name) {
                name_ = field->name;
            }

            if (pValue[index] == NULL) {
                value_ = sql_null;
                return;
            }

            switch (field->type) {
                case MYSQL_TYPE_BIT:
                case MYSQL_TYPE_TINY:
                case MYSQL_TYPE_SHORT:
                case MYSQL_TYPE_LONG:
                case MYSQL_TYPE_INT24:
                case MYSQL_TYPE_LONGLONG:
                    value_ = std::stoll(pValue[index]);
                    break;
                case MYSQL_TYPE_NULL:
                    value_ = sql_null;
                    break;
                case MYSQL_TYPE_TIME:
                case MYSQL_TYPE_YEAR:
                case MYSQL_TYPE_NEWDATE:
                case MYSQL_TYPE_DATE:
                case MYSQL_TYPE_DATETIME:
                case MYSQL_TYPE_TIMESTAMP: {
                    struct tm *tp;

                    if ((tp = getdate(pValue[index]))) {
                        long long epoch = mktime(tp);
                        value_ = epoch;
                    }
                    break;
                }
                case MYSQL_TYPE_VARCHAR:
                case MYSQL_TYPE_VAR_STRING:
                case MYSQL_TYPE_GEOMETRY:
                case MYSQL_TYPE_SET:
                case MYSQL_TYPE_ENUM:
                case MYSQL_TYPE_DECIMAL:
                case MYSQL_TYPE_NEWDECIMAL:
                case MYSQL_TYPE_STRING:
                default: {
                    value_ = pValue[index];
                    break;
                }
                case MYSQL_TYPE_FLOAT:
                case MYSQL_TYPE_DOUBLE:
                    value_ = std::stod(pValue[index]);
                    break;
                case MYSQL_TYPE_TINY_BLOB:
                case MYSQL_TYPE_MEDIUM_BLOB:
                case MYSQL_TYPE_LONG_BLOB:
                case MYSQL_TYPE_BLOB: {
                    auto lengths = mysql_fetch_lengths(res.get());
                    value_ = sql_blob(pValue[index], lengths[index]);
                    break;
                }
            }
        }

        mysql_cached_column::mysql_cached_column(const mysql_cached_column &other) : name_(other.name_), value_(other.value_), type_(other.type_)
        {
        }

        mysql_cached_column::mysql_cached_column(mysql_cached_column &&other)
            : name_(std::move(other.name_)), value_(std::move(other.value_)), type_(other.type_)
        {
            other.value_ = nullptr;
        }
        mysql_cached_column::~mysql_cached_column()
        {
        }

        mysql_cached_column &mysql_cached_column::operator=(const mysql_cached_column &other)
        {
            name_ = other.name_;
            value_ = other.value_;
            type_ = other.type_;

            return *this;
        }
        mysql_cached_column &mysql_cached_column::operator=(mysql_cached_column &&other)
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

        sql_value mysql_cached_column::to_value() const
        {
            return value_;
        }

        int mysql_cached_column::sql_type() const
        {
            return type_;
        }

        string mysql_cached_column::name() const
        {
            return name_;
        }
    }
}

#endif

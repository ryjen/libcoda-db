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
        mysql_column::mysql_column(const shared_ptr<MYSQL_RES> &res, MYSQL_ROW pValue, size_t index) : value_(pValue), res_(res), index_(index)
        {
            if (value_ == nullptr) {
                throw database_exception("no mysql value provided for column");
            }
            if (res_ == nullptr) {
                throw database_exception("no mysql result provided for column");
            }
        }

        mysql_column::mysql_column(mysql_column &&other) : value_(other.value_), res_(other.res_), index_(other.index_)
        {
            other.value_ = NULL;
            other.res_ = nullptr;
        }

        mysql_column::~mysql_column()
        {
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

        /*
         * Key function here.  Handles conversion from MYSQL_ROW to sql value
         * TODO: test more
         */
        sql_value mysql_column::to_value() const
        {
            auto field = mysql_fetch_field_direct(res_.get(), index_);

            if (value_[index_] == nullptr || field == nullptr) {
                throw no_such_column_exception();
            }

            auto lengths = mysql_fetch_lengths(res_.get());

            if (lengths == NULL) {
                throw binding_error("no lengths for field");
            }

            return mysql_data_mapper::to_value(field->type, value_[index_], lengths[index_]);
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

        mysql_stmt_column::mysql_stmt_column(const string &name, const shared_ptr<mysql_binding> &bindings, size_t position)
            : name_(name), value_(bindings), position_(position)
        {
            if (bindings == nullptr) {
                throw database_exception("no mysql bindings provided for column");
            }
        }


        mysql_stmt_column::mysql_stmt_column(mysql_stmt_column &&other)
            : name_(std::move(other.name_)), value_(other.value_), position_(other.position_)
        {
            other.value_ = nullptr;
        }

        mysql_stmt_column::~mysql_stmt_column()
        {
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
    }
}

#endif

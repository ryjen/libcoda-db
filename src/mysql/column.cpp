
#include "column.h"
#include <time.h>
#include <string>
#include "../sql_value.h"
#include "binding.h"

using namespace std;

namespace rj
{
    namespace db
    {
        namespace mysql
        {
            column::column(const shared_ptr<MYSQL_RES> &res, MYSQL_ROW pValue, size_t index)
                : value_(pValue), res_(res), index_(index)
            {
                if (value_ == nullptr) {
                    throw database_exception("no mysql value provided for column");
                }
                if (res_ == nullptr) {
                    throw database_exception("no mysql result provided for column");
                }
            }

            column::column(column &&other) : value_(other.value_), res_(other.res_), index_(other.index_)
            {
                other.value_ = nullptr;
                other.res_ = nullptr;
            }

            column::~column()
            {
            }

            column &column::operator=(column &&other)
            {
                value_ = other.value_;
                index_ = other.index_;
                res_ = other.res_;

                other.value_ = nullptr;
                other.res_ = nullptr;

                return *this;
            }

            bool column::is_valid() const
            {
                return value_ != nullptr;
            }

            /*
             * Key function here.  Handles conversion from MYSQL_ROW to sql value
             * TODO: test more
             */
            sql_value column::to_value() const
            {
                if (res_ == nullptr) {
                    throw no_such_column_exception();
                }

                auto field = mysql_fetch_field_direct(res_.get(), index_);

                if (field == nullptr) {
                    throw no_such_column_exception();
                }

                auto lengths = mysql_fetch_lengths(res_.get());

                return mysql::data_mapper::to_value(field->type, value_[index_], !lengths ? 0 : lengths[index_]);
            }

            int column::sql_type() const
            {
                if (res_ == nullptr) {
                    throw no_such_column_exception();
                }

                auto field = mysql_fetch_field_direct(res_.get(), index_);

                if (field == nullptr) {
                    return MYSQL_TYPE_NULL;
                }

                return field->type;
            }

            string column::name() const
            {
                if (res_ == nullptr) {
                    return string();
                }

                auto field = mysql_fetch_field_direct(res_.get(), index_);

                if (field == nullptr || field->name == nullptr) {
                    return string();
                }

                return field->name;
            }


            /* statement version */

            stmt_column::stmt_column(const string &name, const shared_ptr<mysql::binding> &bindings, size_t position)
                : name_(name), value_(bindings), position_(position)
            {
                if (bindings == nullptr) {
                    throw database_exception("no mysql bindings provided for column");
                }
            }


            stmt_column::stmt_column(stmt_column &&other)
                : name_(std::move(other.name_)), value_(std::move(other.value_)), position_(other.position_)
            {
                other.value_ = nullptr;
            }

            stmt_column::~stmt_column()
            {
            }

            stmt_column &stmt_column::operator=(stmt_column &&other)
            {
                name_ = std::move(other.name_);
                value_ = std::move(other.value_);
                position_ = other.position_;
                other.value_ = nullptr;
                return *this;
            }

            bool stmt_column::is_valid() const
            {
                return value_ != nullptr;
            }

            sql_value stmt_column::to_value() const
            {
                if (value_ == nullptr) {
                    return sql_value();
                }

                return value_->to_value(position_);
            }

            int stmt_column::sql_type() const
            {
                if (value_ == nullptr) {
                    return -1;
                }

                return value_->sql_type(position_);
            }

            string stmt_column::name() const
            {
                return name_;
            }
        }
    }
}

#include "column.h"
#include "log.h"
#include "sql_value.h"

namespace rj
{
    namespace db
    {
        column::column()
        {
        }

        column::column(const std::shared_ptr<column_impl> &impl) : impl_(impl)
        {
            if (impl_ == nullptr) {
                throw database_exception("no implementation for column");
            }
        }

        column::column(const column &other) : impl_(other.impl_)
        {
        }

        column::column(column &&other) : impl_(std::move(other.impl_))
        {
            other.impl_ = nullptr;
        }

        column &column::operator=(const column &other)
        {
            impl_ = other.impl_;

            return *this;
        }

        column &column::operator=(column &&other)
        {
            impl_ = std::move(other.impl_);
            other.impl_ = nullptr;

            return *this;
        }

        bool column::is_valid() const
        {
            if (impl_ == nullptr) {
                return false;
            }

            return impl_->is_valid();
        }

        sql_value column::value() const
        {
            if (impl_ == nullptr) {
                return sql_value();
            }

            return impl_->to_value();
        }

        std::string column::name() const
        {
            if (impl_ == nullptr) {
                return std::string();
            }
            return impl_->name();
        }

        std::shared_ptr<column_impl> column::impl() const
        {
            return impl_;
        }

        column::operator sql_number() const
        {
            return impl_->to_value();
        }
        column::operator sql_time() const
        {
            return impl_->to_value();
        }
        column::operator sql_string() const
        {
            return impl_->to_value();
        }
        column::operator sql_wstring() const
        {
            return impl_->to_value();
        }

        column::operator sql_blob() const
        {
            return impl_->to_value();
        }

        column::operator bool() const
        {
            return impl_->to_value();
        }
        column::operator char() const
        {
            return impl_->to_value();
        }
        column::operator unsigned char() const
        {
            return impl_->to_value();
        }
        column::operator wchar_t() const
        {
            return impl_->to_value();
        }
        column::operator short() const
        {
            return impl_->to_value();
        }
        column::operator unsigned short() const
        {
            return impl_->to_value();
        }
        column::operator int() const
        {
            return impl_->to_value();
        }
        column::operator unsigned int() const
        {
            return impl_->to_value();
        }
        column::operator long() const
        {
            return impl_->to_value();
        }
        column::operator unsigned long() const
        {
            return impl_->to_value();
        }
        column::operator long long() const
        {
            return impl_->to_value();
        }
        column::operator unsigned long long() const
        {
            return impl_->to_value();
        }
        column::operator float() const
        {
            return impl_->to_value();
        }
        column::operator double() const
        {
            return impl_->to_value();
        }
        column::operator long double() const
        {
            return impl_->to_value();
        }

        bool column::operator==(const sql_value &other) const
        {
            return impl_->to_value() == other;
        }

        bool column::operator==(const sql_null_type &other) const
        {
            return impl_->to_value() == other;
        }
        bool column::operator==(const sql_number &other) const
        {
            return impl_->to_value() == other;
        }
        bool column::operator==(const sql_string &other) const
        {
            return impl_->to_value() == other;
        }
        bool column::operator==(const sql_wstring &other) const
        {
            return impl_->to_value() == other;
        }
        bool column::operator==(const sql_time &other) const
        {
            return impl_->to_value() == other;
        }
        bool column::operator==(const sql_blob &other) const
        {
            return impl_->to_value() == other;
        }

        bool column::operator==(const bool &other) const
        {
            return impl_->to_value() == other;
        }
        bool column::operator==(const char &other) const
        {
            return impl_->to_value() == other;
        }
        bool column::operator==(const unsigned char &other) const
        {
            return impl_->to_value() == other;
        }
        bool column::operator==(const wchar_t &other) const
        {
            return impl_->to_value() == other;
        }
        bool column::operator==(const short &other) const
        {
            return impl_->to_value() == other;
        }
        bool column::operator==(const unsigned short &other) const
        {
            return impl_->to_value() == other;
        }
        bool column::operator==(const int &other) const
        {
            return impl_->to_value() == other;
        }
        bool column::operator==(const unsigned int &other) const
        {
            return impl_->to_value() == other;
        }
        bool column::operator==(const long &other) const
        {
            return impl_->to_value() == other;
        }
        bool column::operator==(const unsigned long &other) const
        {
            return impl_->to_value() == other;
        }
        bool column::operator==(const long long &other) const
        {
            return impl_->to_value() == other;
        }
        bool column::operator==(const unsigned long long &other) const
        {
            return impl_->to_value() == other;
        }
        bool column::operator==(const float &other) const
        {
            return impl_->to_value() == other;
        }
        bool column::operator==(const double &other) const
        {
            return impl_->to_value() == other;
        }
        bool column::operator==(const long double &other) const
        {
            return impl_->to_value() == other;
        }

        bool operator==(const sql_value &other, const column &column)
        {
            return other == column.value();
        }
    }
}

#include "column.h"

namespace arg3
{
    namespace db
    {
        column::column()
        {}

        column::column(shared_ptr<column_impl> impl) : impl_(impl)
        {

        }

        column::column(const column &other) : impl_(other.impl_)
        {}

        column::column(column &&other) : impl_(std::move(other.impl_))
        {
            other.impl_ = nullptr;
        }

        column &column::operator=(const column &other)
        {
            if (this != &other)
            {
                impl_ = other.impl_;
            }
            return *this;
        }

        column &column::operator=(column && other)
        {
            if (this != &other)
            {
                impl_ = std::move(other.impl_);
                other.impl_ = nullptr;
            }
            return *this;
        }

        bool column::is_valid() const
        {
            return impl_->is_valid();
        }

        sql_blob column::to_blob() const
        {
            return impl_->to_blob();
        }

        double column::to_double() const
        {
            return impl_->to_double();
        }

        int column::to_int() const
        {
            return impl_->to_int();
        }

        bool sqlite3_column::to_bool() const
        {
            assert_value();

            return sqlite3_value_int(value_);
        }
        bool column::to_bool() const
        {
            return impl_->to_bool();
        }

        int64_t column::to_int64() const
        {
            return impl_->to_int64();
        }

        const wchar_t *column::to_text16() const
        {
            return impl_->to_text16();
        }

        const unsigned char *column::to_text() const
        {
            return impl_->to_text();
        }

        string column::to_string() const
        {
            return impl_->to_string();
        }

        sql_value column::to_value() const
        {
            return impl_->to_value();
        }

        column::operator string() const
        {
            return to_string();
        }

        column::operator int() const
        {
            return to_int();
        }

        column::operator sqlite3_int64() const
        {
            return to_int64();
        }

        column::operator double() const
        {
            return to_double();
        }
    }
}
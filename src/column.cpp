#include "column.h"
#include "log.h"

namespace arg3
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

        sql_value column::to_value() const
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

        sql_blob column::to_blob() const
        {
            return impl_->to_value();
        }

        sql_time column::to_time() const
        {
            return impl_->to_value();
        }

        column::operator int() const
        {
            return impl_->to_value();
        }

        column::operator unsigned() const
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
        column::operator double() const
        {
            return impl_->to_value();
        }

        column::operator float() const
        {
            return impl_->to_value();
        }

        column::operator std::string() const
        {
            return impl_->to_value();
        }

        column::operator std::wstring() const
        {
            return impl_->to_value();
        }
    }
}

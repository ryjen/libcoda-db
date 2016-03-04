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
    }
}

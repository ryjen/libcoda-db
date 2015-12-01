#include "column.h"

namespace arg3
{
    namespace db
    {
        column::column()
        {
        }

        column::column(shared_ptr<column_impl> impl) : impl_(impl)
        {
            assert(impl_ != nullptr);
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
            if (impl_ == nullptr) return false;

            return impl_->is_valid();
        }

        sql_value column::to_value() const
        {
            assert(impl_ != nullptr);

            return impl_->to_value();
        }

        string column::name() const
        {
            assert(impl_ != nullptr);

            return impl_->name();
        }

        shared_ptr<column_impl> column::impl() const
        {
            return impl_;
        }
    }
}
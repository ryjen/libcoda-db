/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "row.h"
#include "resultset.h"

namespace arg3
{
    namespace db
    {
        row::row(shared_ptr<row_impl> impl) : impl_(impl)
        {
            assert(impl_ != nullptr);
        }

        row::row(const row &other) : impl_(other.impl_)
        {}

        row::row(row &&other) : impl_(std::move(other.impl_))
        {
            other.impl_ = nullptr;
        }

        row::~row()
        {
        }

        row &row::operator=(const row &other)
        {
            impl_ = other.impl_;

            return *this;
        }

        row &row::operator=(row && other)
        {
            impl_ = std::move(other.impl_);
            other.impl_ = nullptr;

            return *this;
        }

        row::iterator row::begin()
        {
            return iterator(impl_, 0);
        }

        row::const_iterator row::begin() const
        {
            return const_iterator(impl_, 0);
        }

        row::const_iterator row::cbegin() const
        {
            return begin();
        }

        row::iterator row::end()
        {
            return iterator(impl_, size());
        }

        row::const_iterator row::end() const
        {
            return const_iterator(impl_, size());
        }

        row::const_iterator row::cend() const
        {
            return end();
        }

        row::reverse_iterator row::rbegin()
        {
            return reverse_iterator(end());
        }

        row::const_reverse_iterator row::rbegin() const
        {
            return const_reverse_iterator(end());
        }

        row::const_reverse_iterator row::crbegin() const
        {
            return rbegin();
        }

        row::reverse_iterator row::rend()
        {
            return reverse_iterator(begin());
        }

        row::const_reverse_iterator row::rend() const
        {
            return const_reverse_iterator(begin());
        }

        row::const_reverse_iterator row::crend() const
        {
            return rend();
        }

        column row::operator[](size_t nPosition) const
        {
            return column(nPosition);
        }

        column row::operator[](const string &name) const
        {
            return column(name);
        }


        string row::column_name(size_t nPosition) const
        {
            assert(impl_ != nullptr);
            return impl_->column_name(nPosition);
        }

        arg3::db::column row::column(size_t nPosition) const
        {
            assert(impl_ != nullptr);
            return impl_->column(nPosition);
        }

        arg3::db::column row::column(const string &name) const
        {
            assert(impl_ != nullptr);
            return impl_->column(name);
        }

        size_t row::size() const
        {
            return impl_ == nullptr ? 0 : impl_->size();
        }

        bool row::empty() const
        {
            return size() == 0;
        }

        bool row::is_valid() const
        {
            return impl_ != nullptr && impl_->is_valid();
        }
    }
}
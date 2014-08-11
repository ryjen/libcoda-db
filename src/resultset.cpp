/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "resultset.h"
#include "sqldb.h"

namespace arg3
{
    namespace db
    {

        resultset::resultset(shared_ptr<resultset_impl> impl) : impl_(impl)
        {
            assert(impl_ != nullptr);
        }

        resultset::~resultset()
        {
        }

        resultset::resultset(resultset &&other) : impl_(std::move(other.impl_))
        {
            other.impl_ = nullptr;
        }

        resultset &resultset::operator=(resultset && other)
        {
            impl_ = std::move(other.impl_);
            other.impl_ = nullptr;

            return *this;
        }
        row resultset::operator*()
        {
            return impl_->current_row();
        }

        bool resultset::is_valid() const
        {
            return impl_ != nullptr && impl_->is_valid();
        }

        row resultset::current_row()
        {
            assert(is_valid());
            return impl_->current_row();
        }

        bool resultset::next()
        {
            return impl_->next();
        }


        void resultset::reset()
        {
            impl_->reset();
        }

        size_t resultset::size() const
        {
            return impl_->size();
        }

        void resultset::for_each(std::function<void (row row)> funk)
        {
            for (auto & row : *this)
            {
                funk(row);
            }
        }

        resultset::iterator resultset::begin()
        {
            impl_->reset();

            if (impl_->next())
                return iterator(impl_, 0);
            else
                return end();
        }

        resultset::iterator resultset::end()
        {
            return iterator(impl_, -1);
        }


        resultset_iterator::resultset_iterator(shared_ptr<resultset_impl> rset, int position) : rs_(rset), pos_(position), value_(rset->current_row())
        {
        }

        resultset_iterator::resultset_iterator(const resultset_iterator &other) : rs_(other.rs_), pos_(other.pos_), value_(other.value_)
        {}


        resultset_iterator::resultset_iterator(resultset_iterator &&other) : rs_(std::move(other.rs_)), pos_(other.pos_),
            value_(std::move(other.value_))
        {
            other.rs_ = nullptr;
            other.pos_ = -1;
        }

        resultset_iterator::~resultset_iterator() {}

        resultset_iterator &resultset_iterator::operator=(resultset_iterator && other)
        {
            rs_ = std::move(other.rs_);
            pos_ = other.pos_;
            value_ = std::move(other.value_);
            other.rs_ = nullptr;

            return *this;
        }

        resultset_iterator::reference resultset_iterator::operator*()
        {
            return value_;
        }

        resultset_iterator &resultset_iterator::operator++()
        {
            if (pos_ == -1 || rs_ == nullptr)
                return *this;

            bool res = rs_->next();

            if (res)
            {
                pos_++;
                value_ = rs_->current_row();
            }
            else
            {
                pos_ = -1;
            }

            return *this;

        }


        resultset_iterator::pointer resultset_iterator::operator->()
        {
            return &(operator*());
        }

        resultset_iterator resultset_iterator::operator++(int)
        {
            resultset_iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        resultset_iterator resultset_iterator::operator+(difference_type n)
        {
            resultset_iterator tmp(*this);
            for (difference_type i = 0; i < n; i++)
                ++(tmp);
            return tmp;
        }

        resultset_iterator &resultset_iterator::operator+=(difference_type n)
        {
            for (difference_type i = 0; i < n; i++)
                operator++();
            return *this;
        }

        bool resultset_iterator::operator==(const resultset_iterator &other) const
        {
            return pos_ == other.pos_;
        }

        bool resultset_iterator::operator!=(const resultset_iterator &other) const
        {
            return !operator==(other);
        }

        bool resultset_iterator::operator<(const resultset_iterator &other) const
        {
            if (pos_ == -1 && other.pos_ == -1)
                return false;
            else if (pos_ == -1)
                return false;
            else if (other.pos_ == -1)
                return true;
            else
                return pos_ < other.pos_;
        }

        bool resultset_iterator::operator<=(const resultset_iterator &other) const
        {
            return operator<(other) || operator==(other);
        }

        bool resultset_iterator::operator>(const resultset_iterator &other) const
        {
            return !operator<(other);
        }

        bool resultset_iterator::operator>=(const resultset_iterator &other) const
        {
            return operator>(other) || operator==(other);
        }
    }
}
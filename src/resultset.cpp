/*!
 * @copyright ryan jennings (ryan-jennings.net), 2013
 */
#include "resultset.h"
#include "exception.h"
#include "sqldb.h"

using namespace std;

namespace rj
{
    namespace db
    {
        resultset::resultset(const shared_ptr<resultset_impl> &impl) : impl_(impl)
        {
            if (impl_ == nullptr) {
                throw database_exception("no implementation provided for resultset");
            }
        }

        resultset::~resultset()
        {
        }

        resultset::resultset(resultset &&other) : impl_(std::move(other.impl_))
        {
            other.impl_ = nullptr;
        }

        resultset &resultset::operator=(resultset &&other)
        {
            impl_ = std::move(other.impl_);
            other.impl_ = nullptr;
            return *this;
        }

        resultset_impl::row_type resultset::operator*()
        {
            return impl_->current_row();
        }

        bool resultset::is_valid() const
        {
            return impl_ != nullptr && impl_->is_valid();
        }

        resultset_impl::row_type resultset::current_row()
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
            return distance(begin(), end());
        }

        bool resultset::empty() const
        {
            return begin() == end();
        }

        void resultset::for_each(const std::function<void(const row &row)> &funk) const
        {
            for (auto &row : *this) {
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

        resultset::const_iterator resultset::begin() const
        {
            impl_->reset();

            if (impl_->next())
                return const_iterator(impl_, 0);
            else
                return end();
        }

        resultset::const_iterator resultset::end() const
        {
            return const_iterator(impl_, -1);
        }

        shared_ptr<resultset_impl> resultset::impl() const
        {
            return impl_;
        }
    }
}

/*!
 * @copyright ryan jennings (coda.life), 2013
 */
#include "row.h"
#include "exception.h"

using namespace std;

namespace coda {
    namespace db {
        row::row() : impl_(nullptr) {
        }

        row::row(const shared_ptr<row_impl> &impl) : impl_(impl) {
            if (impl_ == nullptr) {
                throw database_exception("No implementation provided for row");
            }
        }

        row::row(const row &other) : impl_(other.impl_) {
        }

        row::row(row &&other) : impl_(std::move(other.impl_)) {
            other.impl_ = nullptr;
        }

        row::~row() {
        }

        row &row::operator=(const row &other) {
            impl_ = other.impl_;

            return *this;
        }

        row &row::operator=(row &&other) {
            impl_ = std::move(other.impl_);
            other.impl_ = nullptr;

            return *this;
        }

        row::iterator row::begin() {
            return iterator(impl_, 0);
        }

        row::const_iterator row::begin() const {
            return const_iterator(impl_, 0);
        }

        row::const_iterator row::cbegin() const {
            return begin();
        }

        row::iterator row::end() {
            return iterator(impl_, size());
        }

        row::const_iterator row::end() const {
            return const_iterator(impl_, size());
        }

        row::const_iterator row::cend() const {
            return end();
        }

        row::column_type row::operator[](size_t nPosition) const {
            return column(nPosition);
        }

        row::column_type row::operator[](const string &name) const {
            return column(name);
        }

        string row::column_name(size_t nPosition) const {
            assert(impl_ != nullptr);
            return impl_->column_name(nPosition);
        }

        row::column_type row::column(size_t nPosition) const {
            assert(impl_ != nullptr);
            return impl_->column(nPosition);
        }

        row::column_type row::column(const string &name) const {
            assert(impl_ != nullptr);
            return impl_->column(name);
        }

        size_t row::size() const noexcept {
            return impl_ == nullptr ? 0 : impl_->size();
        }

        bool row::empty() const noexcept {
            return size() == 0;
        }

        bool row::is_valid() const noexcept {
            return impl_ != nullptr && impl_->is_valid();
        }

        void row::for_each(const std::function<void( const db::column &)> &funk) const {
            for ( auto &c : *this) {
                funk(c);
            }
        }

        shared_ptr<row_impl> row::impl() const {
            return impl_;
        }

    }
}

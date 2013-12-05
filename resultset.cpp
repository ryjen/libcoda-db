/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "resultset.h"
#include "sqldb.h"

namespace arg3
{
    namespace db
    {
        sqlite3_resultset::sqlite3_resultset(sqlite3_db *db, sqlite3_stmt *stmt) : stmt_(stmt), db_(db), status_(-1)
        {

        }

        sqlite3_resultset::sqlite3_resultset(const sqlite3_resultset &other) : stmt_(other.stmt_), status_(other.status_)
        {}

        sqlite3_resultset::sqlite3_resultset(sqlite3_resultset &&other) : stmt_(std::move(other.stmt_)), status_(other.status_)
        {}

        sqlite3_resultset::~sqlite3_resultset() {}

        sqlite3_resultset &sqlite3_resultset::operator=(const sqlite3_resultset &other)
        {
            if (this != &other)
            {
                stmt_ = other.stmt_;
                status_ = other.status_;
            }

            return *this;
        }

        sqlite3_resultset &sqlite3_resultset::operator=(sqlite3_resultset && other)
        {
            if (this != &other)
            {
                stmt_ = std::move(other.stmt_);
                status_ = other.status_;
            }

            return *this;
        }

        resultset::resultset(shared_ptr<resultset_impl> impl) : impl_(impl)
        {}

        resultset::~resultset()
        {}

        resultset::resultset(const resultset &other) : impl_(other.impl_)
        {

        }
        resultset::resultset(resultset &&other) : impl_(std::move(other.impl_))
        {
            other.impl_ = nullptr;
        }

        resultset &resultset::operator=(const resultset &other)
        {
            if (this != &other)
            {
                impl_ = other.impl_;
            }
            return *this;
        }

        resultset &resultset::operator=(resultset && other)
        {
            if (this != &other)
            {
                impl_ = std::move(other.impl_);
                other.impl_ = nullptr;
            }
            return *this;
        }
        row resultset::operator*()
        {
            return impl_->current_row();
        }

        bool resultset::is_valid() const
        {
            return impl_->is_valid();
        }

        bool sqlite3_resultset::is_valid() const
        {
            return stmt_ != NULL;
        }

        row resultset::current_row()
        {
            return impl_->current_row();
        }

        row sqlite3_resultset::current_row()
        {
            return row(make_shared<sqlite3_row>(db_, stmt_));
        }

        bool resultset::next()
        {
            return impl_->next();
        }

        bool sqlite3_resultset::next()
        {
            if (status_ == SQLITE_DONE)
                return false;

            status_ = sqlite3_step(stmt_);

            return status_ == SQLITE_ROW || status_ == SQLITE_DONE;
        }

        void resultset::reset()
        {
            impl_->reset();
        }

        void sqlite3_resultset::reset()
        {
            if (sqlite3_reset(stmt_) != SQLITE_OK)
                throw database_exception(db_->last_error());
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
        }

        resultset_iterator::~resultset_iterator() {}

        resultset_iterator &resultset_iterator::operator=(const resultset_iterator &other)
        {
            if (this != &other)
            {
                rs_ = other.rs_;
                pos_ = other.pos_;
                value_ = other.value_;
            }
            return *this;
        }

        resultset_iterator &resultset_iterator::operator=(resultset_iterator && other)
        {
            if (this != &other)
            {
                rs_ = std::move(other.rs_);
                pos_ = other.pos_;
                value_ = std::move(other.value_);
                other.rs_ = nullptr;
            }
            return *this;
        }

        resultset_iterator::reference resultset_iterator::operator*()
        {
            return value_;
        }

        resultset_iterator &resultset_iterator::operator++()
        {
            if (rs_ == nullptr)
                return *this;

            bool res = rs_->next();

            if (res)
            {
                pos_++;
                value_ = rs_->current_row();
            }
            else
            {
                rs_ = nullptr;
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
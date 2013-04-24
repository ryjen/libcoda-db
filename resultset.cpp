/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "resultset.h"

namespace arg3
{
    namespace db
    {
        resultset::resultset(sqlite3_stmt *stmt) : stmt_(stmt), status_(-1)
        {

        }

        int resultset::step()
        {

            status_ = sqlite3_step(stmt_);

            return status_;
        }

        /*int resultset::status()
        {
            if(status_ == -1)
                step();

            return status_;
        }*/

        bool resultset::has_more()
        {
            if (status_ == -1)
                step();

            return status_ == SQLITE_ROW;
        }

        bool resultset::is_valid()
        {
            return has_more();
        }

        row resultset::operator*()
        {
            if (status_ == -1)
                step();

            return row(this);
        }

        bool resultset::next()
        {
            return step() == SQLITE_ROW;
        }

        resultset::iterator resultset::begin()
        {
            sqlite3_reset(stmt_);

            if (step() == SQLITE_ROW)
                return iterator(this, 0);
            else
                return end();
        }

        resultset::iterator resultset::end()
        {
            return iterator(this, -1);
        }


        resultset_iterator::resultset_iterator(resultset *rset, int position) : rs_(rset), pos_(position), value_(rset)
        {
        }

        resultset_iterator::reference resultset_iterator::operator*()
        {
            return value_;
        }

        resultset_iterator &resultset_iterator::operator++()
        {
            if (rs_ == NULL)
                return *this;

            int res = rs_->step();

            pos_++;

            switch (res)
            {

            case SQLITE_DONE:
                rs_ = NULL;
                pos_ = -1;
                break;
            case SQLITE_ROW:
                value_ = row(rs_);
                break;
            default:
                throw database_exception();
                break;
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
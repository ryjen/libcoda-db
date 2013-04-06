/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "row.h"
#include "resultset.h"

namespace arg3
{
    namespace db
    {
        row::row(resultset *results) : results_(results)
        {
            assert(results_ != NULL);

            assert(results_->stmt_ != NULL);

            size_ = sqlite3_column_count(results_->stmt_);
        }

        row::iterator row::begin()
        {
            return iterator(this, 0);
        }

        row::const_iterator row::begin() const
        {
            return const_iterator(this, 0);
        }

        row::const_iterator row::cbegin() const
        {
            return begin();
        }

        row::iterator row::end()
        {
            return iterator(this, size());
        }

        row::const_iterator row::end() const
        {
            return const_iterator(this, size());
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
            return column_value(nPosition);
        }

        column row::operator[](const string &name) const
        {
            return column_value(name);
        }

        column row::column_value(size_t nPosition) const
        {
            assert(nPosition < size());

            return db::column( sqlite3_column_value(results_->stmt_, nPosition ) );
        }

        column row::column_value(const string &name) const
        {
            assert(!name.empty());

            for(size_t i = 0, size = sqlite3_column_count(results_->stmt_); i < size; i++)
            {
                const char *col_name = sqlite3_column_name(results_->stmt_, i);

                if(name == col_name)
                {
                    return column_value(i);
                }
            }
            return column();
        }

        string row::column_name(size_t nPosition) const
        {
            assert(nPosition < size());

            return sqlite3_column_name(results_->stmt_, nPosition);
        }

        size_t row::size() const
        {
            return size_;
        }

        bool row::empty() const
        {
            return size() == 0;
        }
    }
}
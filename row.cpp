/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "row.h"
#include "resultset.h"

namespace arg3
{
    namespace db
    {
        row::row(resultset *results) : m_results(results)
        {
            assert(m_results != NULL);

            assert(m_results->m_stmt != NULL);

            m_size = sqlite3_column_count(m_results->m_stmt);
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

            return db::column( sqlite3_column_value(m_results->m_stmt, nPosition ) );
        }

        column row::column_value(const string &name) const
        {
            assert(!name.empty());

            for(size_t i = 0, size = sqlite3_column_count(m_results->m_stmt); i < size; i++)
            {
                const char *col_name = sqlite3_column_name(m_results->m_stmt, i);

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

            return sqlite3_column_name(m_results->m_stmt, nPosition);
        }

        size_t row::size() const
        {
            return m_size;
        }

        bool row::empty() const
        {
            return size() == 0;
        }
    }
}
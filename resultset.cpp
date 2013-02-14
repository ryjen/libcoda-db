/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "resultset.h"

namespace arg3
{
    namespace db
    {
        resultset::resultset(sqlite3_stmt *stmt) : m_stmt(stmt), m_status(-1)
        {

        }

        void resultset::step()
        {

            m_status = sqlite3_step(m_stmt);
        }

        int resultset::status()
        {
            if(m_status == -1)
                step();

            return m_status;
        }

        resultset::iterator resultset::begin()
        {
            sqlite3_reset(m_stmt);

            step();

            return iterator(this, m_status == SQLITE_ROW ? 0 : -1);
        }

        resultset::iterator resultset::end()
        {
            return iterator(this, -1);
        }


        resultset_iterator::resultset_iterator(resultset *rset, int position) : m_results(rset),
            m_position(position), m_row(m_results)
        {
        }

        resultset_iterator::reference resultset_iterator::operator*()
        {
            return m_row;
        }

        resultset_iterator::self_type &resultset_iterator::operator++()
        {
            if (m_position == -1)
                return *this;

            int res = sqlite3_step(m_results->m_stmt);

            switch (res)
            {

            case SQLITE_DONE:
                m_position = -1;
                break;
            case SQLITE_ROW:
                ++m_position;
                m_row = row(m_results);
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

        resultset_iterator::self_type resultset_iterator::operator++(int)
        {
            self_type tmp(*this);
            ++(*this);
            return tmp;
        }

        resultset_iterator::self_type resultset_iterator::operator+(resultset_iterator::difference_type n)
        {
            self_type tmp(*this);
            for (int i = 0; i < n; i++)
                ++(tmp);
            return tmp;
        }

        resultset_iterator::self_type &resultset_iterator::operator+=(resultset_iterator::difference_type n)
        {
            for (int i = 0; i < n; i++)
                operator++();
            return *this;
        }

        bool resultset_iterator::operator==(const resultset_iterator::self_type &other) const
        {
            return m_position == other.m_position;
        }

        bool resultset_iterator::operator!=(const resultset_iterator::self_type &other) const
        {
            return !operator==(other);
        }

        bool resultset_iterator::operator<(const resultset_iterator::self_type &other) const
        {
            if (m_position == -1 && other.m_position == -1)
                return false;
            else if (m_position == -1)
                return false;
            else if (other.m_position == -1)
                return true;
            else
                return m_position < other.m_position;
        }

        bool resultset_iterator::operator<=(const resultset_iterator::self_type &other) const
        {
            return operator<(other) || operator==(other);
        }

        bool resultset_iterator::operator>(const resultset_iterator::self_type &other) const
        {
            return !operator<(other);
        }

        bool resultset_iterator::operator>=(const resultset_iterator::self_type &other) const
        {
            return operator>(other) || operator==(other);
        }
    }
}
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

        int resultset::step()
        {

            m_status = sqlite3_step(m_stmt);

            return m_status;
        }

        int resultset::status()
        {
            if(m_status == -1)
                step();

            return m_status;
        }

        bool resultset::has_more() {
            if(m_status == -1)
                step();

            return m_status == SQLITE_ROW;
        }
        
        resultset::iterator resultset::begin()
        {
            sqlite3_reset(m_stmt);

            step();

            return iterator(this, 0);
        }

        resultset::iterator resultset::end()
        {
            return iterator(this, -1);
        }


        resultset_iterator::resultset_iterator(resultset *rset, int position) : m_rs(rset), m_pos(position), m_value(rset)
        {
        }

        resultset_iterator::reference resultset_iterator::operator*()
        {
            return m_value;
        }

        resultset_iterator &resultset_iterator::operator++()
        {
            if (m_rs == NULL)
                return *this;

            int res = m_rs->step();

            m_pos++;

            switch (res)
            {

            case SQLITE_DONE:
                m_rs = NULL;
                m_pos = -1;
                break;
            case SQLITE_ROW:
                m_value = row(m_rs);
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
            return m_pos == other.m_pos;
        }

        bool resultset_iterator::operator!=(const resultset_iterator &other) const
        {
            return !operator==(other);
        }

        bool resultset_iterator::operator<(const resultset_iterator &other) const
        {
            if (m_pos == -1 && other.m_pos == -1)
                return false;
            else if (m_pos == -1)
                return false;
            else if (other.m_pos == -1)
                return true;
            else
                return m_pos < other.m_pos;
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
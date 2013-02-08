#include "row.h"

namespace arg3
{
    namespace db
    {
        row::row(sqlite3_stmt *stmt) : m_stmt(stmt), m_size(0)
        {
            m_size = sqlite3_column_count(m_stmt);
        }

        //Methods
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
            return iterator(this, static_cast<int>(size()));
        }

        row::const_iterator row::end() const
        {
            return const_iterator(this, static_cast<int>(size()));
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

        row::reference row::operator[](size_type nPosition) const
        {
        	return column_value(nPosition);
        }

        row::reference row::column_value(size_type nPosition) const 
        {
            //Validate our parameters
            assert(m_stmt != NULL);

            assert(nPosition < size());

            //Update the cached value
            m_value = db::column_value( 
            	sqlite3_column_value(m_stmt, static_cast<int>(nPosition) ) );

            //Return the cached value
            return m_value;
        }

        string row::column_name(size_type nPosition) const 
        {
        	assert(m_stmt != NULL);

        	assert(nPosition < size());

        	return sqlite3_column_name(m_stmt, static_cast<int>(nPosition));
        }

        row::size_type row::size() const
        {
            return m_size;
        }

        bool row::empty() const
        {
            return size() == 0;
        }
    }
}
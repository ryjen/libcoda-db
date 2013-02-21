/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_ROW_H_
#define _ARG3_DB_ROW_H_

#include <sqlite3.h>
#include "column.h"
#include <iterator>

namespace arg3
{
    namespace db
    {

        template<class ValueType, class NonConst, class RowType>
        class row_iterator : public std::iterator<std::random_access_iterator_tag, ValueType>
        {
        protected:
            RowType *m_row;
            int m_position;

            ValueType get_value(int nPosition) {

            }
        public:
            row_iterator() : m_row(NULL),
                m_position(-1)
            {
            }

            row_iterator(RowType *pRow, int nPosition) : m_row(pRow),
                m_position(nPosition)
            {
            }

            ValueType operator*()
            {
                return operator[](m_position);
            }

            ValueType* operator->()
            {
                static NonConst operator_pointer;

                operator_pointer = operator[](m_position);

                return &operator_pointer;
            }

            ValueType operator[](size_t nPosition)
            {
               
                assert(m_row != NULL);

               return m_row->operator[](nPosition);
            }

            row_iterator &operator++()
            {
                ++m_position;
                return *this;
            }

            row_iterator operator++(int)
            {
                row_iterator tmp(*this);
                ++(*this);
                return tmp;
            }

            row_iterator &operator--()
            {
                --m_position;
                return *this;
            }

            row_iterator operator--(int)
            {
                row_iterator tmp(*this);
                --(*this);
                return tmp;
            }

            row_iterator operator+(int n)
            {
                row_iterator tmp(*this);
                tmp.m_position += n;
                return tmp;
            }

            row_iterator &operator+=(int n)
            {
                m_position += n;
                return *this;
            }

            row_iterator operator-(int n)
            {
                row_iterator tmp(*this);
                tmp.m_position -= n;
                return tmp;
            }

            row_iterator &operator-=(int n)
            {
                m_position -= n;
                return *this;
            }

            bool operator==(const row_iterator &other) const
            {
                return m_position == other.m_position;
            }

            bool operator!=(const row_iterator &other) const
            {
                return !operator==(other);
            }

            bool operator<(const row_iterator &other) const
            {
                return m_position < other.m_position;
            }

            bool operator<=(const row_iterator &other) const
            {
                return operator<(other) || operator==(other);
            }

            bool operator>(const row_iterator &other) const
            {
                return !operator<(other);
            }

            bool operator>=(const row_iterator &other) const
            {
                return operator>(other) || operator==(other);
            }

            int operator-(const row_iterator &other)
            {
                return m_position - other.m_position;
            }

            string name() const
            {
                return m_row->column_name(m_position);
            }
        };


        class resultset;

        class row
        {
            friend class resultset_iterator;
            friend class resultset;
        public:
            //Typedefs
            typedef row_iterator<column, column, row>               iterator;
            typedef row_iterator<const column, column, const row>   const_iterator;
            typedef std::reverse_iterator<iterator>                      reverse_iterator;
            typedef std::reverse_iterator<const_iterator>                const_reverse_iterator;
            
        private:
            resultset *m_results;
            size_t m_size;

            row(resultset *results);

        public:

            //Methods
            iterator begin();

            const_iterator begin() const;

            const_iterator cbegin() const;

            iterator end();

            const_iterator end() const;

            const_iterator cend() const;

            reverse_iterator rbegin();

            const_reverse_iterator rbegin() const;

            const_reverse_iterator crbegin() const;

            reverse_iterator rend();

            const_reverse_iterator rend() const;

            const_reverse_iterator crend() const;

            column operator[](size_t nPosition) const;

            column operator[](const string &name) const;

            string column_name(size_t nPosition) const;

            column column_value(size_t nPosition) const;

            column column_value(const string &name) const;

            size_t size() const;

            bool empty() const;
        };
    }
}

#endif

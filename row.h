/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_ROW_H_
#define _ARG3_DB_ROW_H_

#include <sqlite3.h>
#include "column_value.h"
#include <iterator>

namespace arg3
{
    namespace db
    {

        template<class _Ty, class _non_const_Ty, class TRow>
        class column_iterator : public std::iterator<std::random_access_iterator_tag, _Ty>
        {
        public:
            //Typedefs
            typedef const column_iterator const_self_type;
            typedef column_iterator       self_type;
            typedef size_t                size_type;
            typedef _non_const_Ty            &reference;
            typedef _non_const_Ty            *pointer;
            typedef int                   difference_type;

            //Constructors / Destructors
            column_iterator() : m_row(NULL),
                m_position(-1)
            {
            }

            column_iterator(const_self_type &other) : m_row(other.m_row),
                m_position(other.m_position)
            {
            }

            column_iterator(TRow *pRow, int nPosition) : m_row(pRow),
                m_position(nPosition)
            {
            }

            //Methods
            reference operator*()
            {
                return operator[](m_position);
            }

            self_type &operator=(const_self_type &other)
            {
                m_row = other.m_row;
                m_position = other.m_position;

                return *this;
            }

            pointer operator->()
            {
                return &(operator*());
            }

            reference operator[](size_type nPosition)
            {
                //Validate our parameters
                assert(m_row != NULL);

                return m_row->operator[](nPosition);
            }

            self_type &operator++()
            {
                ++m_position;
                return *this;
            }

            self_type operator++(int)
            {
                self_type tmp(*this);
                ++(*this);
                return tmp;
            }

            self_type &operator--()
            {
                --m_position;
                return *this;
            }

            self_type operator--(int)
            {
                self_type tmp(*this);
                --(*this);
                return tmp;
            }

            self_type operator+(difference_type n)
            {
                self_type tmp(*this);
                tmp.m_position += n;
                return tmp;
            }

            self_type &operator+=(difference_type n)
            {
                m_position += n;
                return *this;
            }

            self_type operator-(difference_type n)
            {
                self_type tmp(*this);
                tmp.m_position -= n;
                return tmp;
            }

            self_type &operator-=(difference_type n)
            {
                m_position -= n;
                return *this;
            }

            bool operator==(const_self_type &other) const
            {
                return m_position == other.m_position;
            }

            bool operator!=(const_self_type &other) const
            {
                return !operator==(other);
            }

            bool operator<(const_self_type &other) const
            {
                return m_position < other.m_position;
            }

            bool operator<=(const_self_type &other) const
            {
                return operator<(other) || operator==(other);
            }

            bool operator>(const_self_type &other) const
            {
                return !operator<(other);
            }

            bool operator>=(const_self_type &other) const
            {
                return operator>(other) || operator==(other);
            }

            difference_type operator-(const_self_type &other)
            {
                return m_position - other.m_position;
            }

            string name() const
            {
                return m_row->column_name(m_position);
            }
        protected:
            //Member variables
            TRow           *m_row;
            difference_type m_position;
        };


        class resultset;

        class row
        {
            friend class resultset_iterator;
            friend class resultset;
        public:
            //Typedefs
            typedef column_iterator<column_value, column_value, row>                          iterator;
            typedef column_iterator<const column_value, column_value, const row>              const_iterator;
            typedef std::reverse_iterator<column_iterator<column_value, column_value, row> >  reverse_iterator;
            typedef std::reverse_iterator<column_iterator<const column_value, column_value, const row> > const_reverse_iterator;
            typedef column_value                                                                    value_type;
            typedef column_value                                                                   &reference;
            typedef const column_value                                                             &const_reference;
            typedef size_t                                                                    size_type;
            typedef iterator::difference_type                                                 difference_type;
        private:
            resultset *m_results;
            size_t m_size;
            mutable value_type m_value;
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

            reference operator[](size_type nPosition) const;

            reference operator[](const string &name) const;

            string column_name(size_type nPosition) const;

            reference column_value(size_type nPosition) const;

            reference column_value(const string &name) const;

            size_type size() const;

            bool empty() const;
        };
    }
}

#endif

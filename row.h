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
            RowType *row_;
            int position_;

            ValueType get_value(int nPosition)
            {

            }
        public:
            row_iterator() : row_(NULL),
                position_(-1)
            {
            }

            row_iterator(RowType *pRow, int nPosition) : row_(pRow),
                position_(nPosition)
            {
            }

            ValueType operator*()
            {
                return operator[](position_);
            }

            ValueType *operator->()
            {
                static NonConst operator_pointer;

                operator_pointer = operator[](position_);

                return &operator_pointer;
            }

            ValueType operator[](size_t nPosition)
            {

                assert(row_ != NULL);

                return row_->operator[](nPosition);
            }

            row_iterator &operator++()
            {
                ++position_;
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
                --position_;
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
                tmp.position_ += n;
                return tmp;
            }

            row_iterator &operator+=(int n)
            {
                position_ += n;
                return *this;
            }

            row_iterator operator-(int n)
            {
                row_iterator tmp(*this);
                tmp.position_ -= n;
                return tmp;
            }

            row_iterator &operator-=(int n)
            {
                position_ -= n;
                return *this;
            }

            bool operator==(const row_iterator &other) const
            {
                return position_ == other.position_;
            }

            bool operator!=(const row_iterator &other) const
            {
                return !operator==(other);
            }

            bool operator<(const row_iterator &other) const
            {
                return position_ < other.position_;
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
                return position_ - other.position_;
            }

            string name() const
            {
                return row_->column_name(position_);
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
            resultset *results_;
            size_t size_;

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

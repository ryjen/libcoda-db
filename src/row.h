
#ifndef _ARG3_DB_ROW_H_
#define _ARG3_DB_ROW_H_

#include <sqlite3.h>
#include "column.h"
#include <iterator>

namespace arg3
{
    namespace db
    {
        /*!
         * Implementation spefic interface for a row
         */
        class row_impl
        {
        public:
            row_impl() = default;
            row_impl(const row_impl &other) = default;
            row_impl(row_impl &&other) = default;
            row_impl &operator=(const row_impl &other) = default;
            row_impl &operator=(row_impl && other) = default;
            virtual ~row_impl() = default;

            virtual string column_name(size_t nPosition) const = 0;

            virtual arg3::db::column column(size_t nPosition) const = 0;

            virtual arg3::db::column column(const string &name) const = 0;

            virtual size_t size() const = 0;

            virtual bool is_valid() const = 0;

        };

        /*!
         * iterator for columns in a row
         */
        template<class ValueType, class NonConst, class RowType>
        class row_iterator : public std::iterator<std::random_access_iterator_tag, ValueType>
        {
        protected:
            shared_ptr<RowType> row_;
            int position_;
        public:
            row_iterator() : row_(NULL),
                position_(-1)
            {
            }

            row_iterator(shared_ptr<RowType> pRow, int nPosition) : row_(pRow),
                position_(nPosition)
            {
            }

            row_iterator(const row_iterator &other) : row_(other.row_), position_(other.position_)
            {
            }

            row_iterator(row_iterator &&other) : row_(std::move(other.row_)), position_(other.position_)
            {
                other.row_ = nullptr;
            }

            virtual ~row_iterator() {}

            row_iterator &operator=(const row_iterator &other)
            {
                if (this != &other)
                {
                    row_ = other.row_;
                    position_ = other.position_;
                }
                return this;
            }

            row_iterator &operator=(row_iterator && other)
            {
                if (this != &other)
                {
                    row_ = std::move(other.row_);
                    position_ = other.position_;
                    other.row_ = nullptr;
                }
                return this;
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

                return row_->column(nPosition);
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

        /*!
         * represent a row in a table
         * implementation is specific to a type of database
         */
        class row
        {
        private:
            shared_ptr<row_impl> impl_;

        public:
            typedef row_iterator<column, column, row_impl> iterator;
            typedef row_iterator<const column, column, const row_impl> const_iterator;
            typedef std::reverse_iterator<iterator> reverse_iterator;
            typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

            row(shared_ptr<row_impl> impl);
            row(const row &other);
            row(row &&other);
            virtual ~row();

            row &operator=(const row &other);
            row &operator=(row && other);

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

            arg3::db::column column(size_t nPosition) const;

            arg3::db::column column(const string &name) const;

            size_t size() const;

            bool empty() const;

            bool is_valid() const;
        };

    }
}

#endif

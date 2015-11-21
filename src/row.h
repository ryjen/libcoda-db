
#ifndef ARG3_DB_ROW_H
#define ARG3_DB_ROW_H

#include "column.h"
#include <iterator>
#include <memory>

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
            typedef arg3::db::column column_type;
            row_impl() = default;
            row_impl(const row_impl &other) = default;
            row_impl(row_impl &&other) = default;
            row_impl &operator=(const row_impl &other) = default;
            row_impl &operator=(row_impl &&other) = default;
            virtual ~row_impl() = default;

            virtual string column_name(size_t nPosition) const = 0;

            virtual column_type column(size_t nPosition) const = 0;

            virtual column_type column(const string &name) const = 0;

            virtual size_t size() const = 0;

            virtual bool is_valid() const = 0;
        };

        /*!
         * iterator for columns in a row
         */
        template <class ValueType, class NonConst, class RowType>
        class row_iterator : public std::iterator<std::random_access_iterator_tag, ValueType>
        {
           protected:
            shared_ptr<RowType> row_;
            int position_;
            NonConst currentValue_;
            void set_current_value(size_t index)
            {
                assert(row_ != nullptr);

                if (index >= 0 && index < row_->size()) currentValue_ = row_->column(index);
            }

           public:
            row_iterator() : row_(nullptr), position_(-1)
            {
            }

            row_iterator(shared_ptr<RowType> pRow, int nPosition) : row_(pRow), position_(nPosition)
            {
            }

            row_iterator(const row_iterator &other) : row_(other.row_), position_(other.position_)
            {
            }

            row_iterator(row_iterator &&other) : row_(std::move(other.row_)), position_(other.position_)
            {
                other.row_ = nullptr;
            }

            virtual ~row_iterator()
            {
            }

            row_iterator &operator=(const row_iterator &other)
            {
                row_ = other.row_;
                position_ = other.position_;

                return *this;
            }

            row_iterator &operator=(row_iterator &&other)
            {
                row_ = std::move(other.row_);
                position_ = other.position_;
                other.row_ = nullptr;

                return *this;
            }

            ValueType &operator*()
            {
                set_current_value(position_);
                return currentValue_;
            }

            ValueType *operator->()
            {
                set_current_value(position_);
                return &currentValue_;
            }

            ValueType operator[](size_t nPosition)
            {
                set_current_value(nPosition);
                return currentValue_;
            }

            row_iterator &operator++()
            {
                if (position_ < row_->size()) ++position_;
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
                if (position_ > 0) --position_;
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
                tmp += n;
                return tmp;
            }

            row_iterator &operator+=(int n)
            {
                position_ = std::min(position_ + n, row_->size());
                return *this;
            }

            row_iterator operator-(int n)
            {
                row_iterator tmp(*this);
                tmp -= n;
                return tmp;
            }

            row_iterator &operator-=(int n)
            {
                position_ = std::max(position_ - n, 0);
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
                if (position_ >= other.position_)
                    return position_ - other.position_;
                else
                    return 0;
            }

            string name() const
            {
                assert(row_ != nullptr);
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
            typedef arg3::db::column column_type;
            typedef row_iterator<column_type, column_type, row_impl> iterator;
            typedef row_iterator<const column_type, column_type, const row_impl> const_iterator;

            row();
            row(shared_ptr<row_impl> impl);
            row(const row &other);
            row(row &&other);
            virtual ~row();

            row &operator=(const row &other);
            row &operator=(row &&other);

            // Methods
            iterator begin();

            const_iterator begin() const;

            const_iterator cbegin() const;

            iterator end();

            const_iterator end() const;

            const_iterator cend() const;

            column_type operator[](size_t nPosition) const;

            column_type operator[](const string &name) const;

            string column_name(size_t nPosition) const;

            column_type column(size_t nPosition) const;

            column_type column(const string &name) const;

            size_t size() const;

            bool empty() const;

            bool is_valid() const;

            void for_each(std::function<void(const db::column &)> funk) const;

            shared_ptr<row_impl> impl() const;
        };
    }
}

#endif

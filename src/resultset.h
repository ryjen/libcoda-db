/*!
 * @file resultset.h
 * a query result set in a database
 */
#ifndef ARG3_DB_RESULTSET_H
#define ARG3_DB_RESULTSET_H

#include <memory>
#include "exception.h"
#include "row.h"

namespace arg3
{
    namespace db
    {
        class resultset;

        /*!
         * an interface for a database specific set of results for a query
         */
        class resultset_impl
        {
           public:
            typedef arg3::db::row row_type;

           protected:
            resultset_impl() = default;

           public:
            /* non-copyable */
            resultset_impl(const resultset_impl &other) = delete;
            resultset_impl(resultset_impl &&other) = default;
            virtual ~resultset_impl() = default;
            resultset_impl &operator=(const resultset_impl &other) = delete;
            resultset_impl &operator=(resultset_impl &&other) = default;

            /*!
             * tests if this implementation is valid
             * @return true if operations can be performed safely
             */
            virtual bool is_valid() const = 0;

            /*!
             * moves to the next row in the results
             * @return true if successful
             */
            virtual bool next() = 0;

            /*!
             * gets the current row in the results
             * @return the current row object
             */
            virtual row_type current_row() = 0;

            /*!
             * resets this resultset back to the first row
             */
            virtual void reset() = 0;
        };

        /*!
         * an iterator for a rows in a set of results
         */
        template <typename ValueType, typename NonConst>
        class resultset_iterator : public std::iterator<std::input_iterator_tag, ValueType>
        {
           private:
            std::shared_ptr<resultset_impl> rs_;
            int pos_;
            NonConst value_;

           public:
            resultset_iterator(const std::shared_ptr<resultset_impl> &rset, int position) : rs_(rset), pos_(position)
            {
                if (position >= 0) {
                    value_ = rset->current_row();
                } else {
                    value_ = NonConst();
                }
            }

            resultset_iterator(const resultset_iterator &other) : rs_(other.rs_), pos_(other.pos_), value_(other.value_)
            {
            }


            resultset_iterator(resultset_iterator &&other) : rs_(std::move(other.rs_)), pos_(other.pos_), value_(std::move(other.value_))
            {
                other.rs_ = nullptr;
                other.pos_ = -1;
            }

            ~resultset_iterator()
            {
            }

            resultset_iterator &operator=(resultset_iterator &&other)
            {
                rs_ = std::move(other.rs_);
                pos_ = other.pos_;
                value_ = std::move(other.value_);
                other.rs_ = nullptr;
                other.pos_ = -1;

                return *this;
            }

            ValueType &operator*()
            {
                return value_;
            }

            resultset_iterator &operator++()
            {
                if (pos_ == -1 || rs_ == nullptr) {
                    return *this;
                }

                bool res = rs_->next();

                if (res) {
                    pos_++;
                    value_ = rs_->current_row();
                } else {
                    pos_ = -1;
                    value_ = NonConst();
                }

                return *this;
            }


            ValueType *operator->()
            {
                return &(operator*());
            }

            resultset_iterator operator++(int)
            {
                resultset_iterator tmp(*this);
                ++(*this);
                return tmp;
            }

            resultset_iterator operator+(int n)
            {
                resultset_iterator tmp(*this);

                tmp += n;

                return tmp;
            }

            resultset_iterator &operator+=(int n)
            {
                for (int i = 0; i < n; i++) {
                    operator++();
                }
                return *this;
            }

            bool operator==(const resultset_iterator &other) const
            {
                return pos_ == other.pos_;
            }

            bool operator!=(const resultset_iterator &other) const
            {
                return !operator==(other);
            }

            bool operator<(const resultset_iterator &other) const
            {
                if (pos_ == -1 && other.pos_ == -1)
                    return false;
                else if (pos_ == -1)
                    return false;
                else if (other.pos_ == -1)
                    return true;
                else
                    return pos_ < other.pos_;
            }

            bool operator<=(const resultset_iterator &other) const
            {
                return operator<(other) || operator==(other);
            }

            bool operator>(const resultset_iterator &other) const
            {
                return !operator<(other);
            }

            bool operator>=(const resultset_iterator &other) const
            {
                return operator>(other) || operator==(other);
            }
        };

        /*!
         * the results (a set of rows) for a query
         */
        class resultset
        {
           public:
            typedef arg3::db::row row_type;

           private:
            std::shared_ptr<resultset_impl> impl_;

           public:
            typedef resultset_iterator<row_type, row_type> iterator;
            typedef resultset_iterator<const row_type, row_type> const_iterator;

            /*!
             * @param impl the implementation for this resultset
             */
            resultset(const std::shared_ptr<resultset_impl> &impl);

            /* non-copyable */
            resultset(const resultset &other) = delete;
            resultset(resultset &&other);
            virtual ~resultset();
            resultset &operator=(const resultset &other) = delete;
            resultset &operator=(resultset &&other);

            /*!
             * tests if the result set is valid
             * @return true if the implementation is valid
             */
            bool is_valid() const;

            /*!
             * @return an iterator to the first row
             */
            iterator begin();

            /*!
             * @return an immutable iterator to the first row
             */
            const_iterator begin() const;

            /*!
             * @return an iterator to after the last row
             */
            iterator end();

            /*!
             * @return a immutable iterator the after the last row
             */
            const_iterator end() const;

            /*!
             * gets the current row
             * @return the current row
             */
            row_type current_row();

            /*!
             * moves to the next row
             * @return true if successful
             */
            bool next();

            /*!
             * dereference operator to the current row
             */
            row_type operator*();

            /*!
             * resets this result set back to the first row
             */
            void reset();

            /*!
             * @return the number of rows in the result set
             */
            size_t size() const;

            /*!
             * tests if the are no results
             * @return true if there are no results
             */
            bool empty() const;

            /*!
             * @param funk the callback to perform for each row
             */
            void for_each(const std::function<void(const row_type &)> &funk) const;

            /*!
             * @return a pointer to the implementation
             */
            std::shared_ptr<resultset_impl> impl() const;
        };
    }
}

#endif

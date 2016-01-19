
#ifndef ARG3_DB_RESULTSET_H
#define ARG3_DB_RESULTSET_H

#include "row.h"
#include "exception.h"
#include <memory>

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
           protected:
            resultset_impl() = default;

           public:
            resultset_impl(const resultset_impl &other) = delete;
            resultset_impl(resultset_impl &&other) = default;
            virtual ~resultset_impl() = default;

            resultset_impl &operator=(const resultset_impl &other) = delete;
            resultset_impl &operator=(resultset_impl &&other) = default;

            virtual bool is_valid() const = 0;

            virtual bool next() = 0;

            virtual row current_row() = 0;

            virtual void reset() = 0;

            virtual size_t size() const = 0;
        };

        /*!
         * an iterator for a rows in a set of results
         */
        template <typename ValueType, typename NonConst>
        class resultset_iterator : public std::iterator<std::input_iterator_tag, ValueType>
        {
           private:
            shared_ptr<resultset_impl> rs_;
            int pos_;
            NonConst value_;

           public:
            resultset_iterator(const shared_ptr<resultset_impl> &rset, int position) : rs_(rset), pos_(position), value_(rset->current_row())
            {
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
                if (pos_ == -1 || rs_ == nullptr) return *this;

                bool res = rs_->next();

                if (res) {
                    pos_++;
                    value_ = rs_->current_row();
                } else {
                    pos_ = -1;
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
                for (int i = 0; i < n; i++) operator++();
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
           private:
            std::shared_ptr<resultset_impl> impl_;

           public:
            resultset(const std::shared_ptr<resultset_impl> &impl);

            resultset(const resultset &other) = delete;
            resultset(resultset &&other);
            virtual ~resultset();

            resultset &operator=(const resultset &other) = delete;
            resultset &operator=(resultset &&other);

            typedef resultset_iterator<row, row> iterator;
            typedef resultset_iterator<const row, row> const_iterator;

            bool is_valid() const;

            iterator begin();

            const_iterator begin() const;

            iterator end();

            const_iterator end() const;

            row current_row();

            bool next();

            row operator*();

            void reset();

            size_t size() const;

            void for_each(const std::function<void(const row &)> &funk) const;

            std::shared_ptr<resultset_impl> impl() const;
        };
    }
}

#endif

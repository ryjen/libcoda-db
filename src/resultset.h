
#ifndef _ARG3_DB_RESULTSET_H_
#define _ARG3_DB_RESULTSET_H_

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
            resultset_impl &operator=(resultset_impl && other) = default;

            virtual bool is_valid() const = 0;

            virtual bool next() = 0;

            virtual row current_row() = 0;

            virtual void reset() = 0;

            virtual size_t size() const = 0;
        };

        /*!
         * an iterator for a rows in a set of results
         */
        class resultset_iterator : public std::iterator<std::input_iterator_tag, row>
        {
        private:
            shared_ptr<resultset_impl> rs_;
            int pos_;
            row value_;
        public:

            resultset_iterator(shared_ptr<resultset_impl> rs, int position);

            resultset_iterator(const resultset_iterator &other);

            resultset_iterator(resultset_iterator &&other);

            virtual ~resultset_iterator();

            resultset_iterator &operator=(const resultset_iterator &other);

            resultset_iterator &operator=(resultset_iterator && other);

            reference operator*();

            const reference operator*() const;

            pointer operator->();

            const pointer operator->() const;

            resultset_iterator &operator++();

            resultset_iterator operator++(int);

            resultset_iterator operator+(difference_type n);

            resultset_iterator &operator+=(difference_type n);

            bool operator==(const resultset_iterator &other) const;

            bool operator!=(const resultset_iterator &other) const;

            bool operator<(const resultset_iterator &other) const;

            bool operator<=(const resultset_iterator &other) const;

            bool operator>(const resultset_iterator &other) const;

            bool operator>=(const resultset_iterator &other) const;

        };

        /*!
         * the results (a set of rows) for a query
         */
        class resultset
        {
        private:
            shared_ptr<resultset_impl> impl_;
        public:

            resultset(shared_ptr<resultset_impl> impl);

            resultset(const resultset &other) = delete;
            resultset(resultset &&other);
            virtual ~resultset();

            resultset &operator=(const resultset &other) = delete;
            resultset &operator=(resultset && other);

            typedef resultset_iterator iterator;

            bool is_valid() const;

            iterator begin();

            iterator end();

            row current_row();

            bool next();

            row operator*();

            void reset();

            size_t size() const;
        };

    }


}

#endif

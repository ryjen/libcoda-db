/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
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
        class sqlite3_db;

        class resultset_impl
        {
        protected:
            resultset_impl() = default;
        public:
            resultset_impl(const resultset_impl &other) = default;
            resultset_impl(resultset_impl &&other) = default;
            virtual ~resultset_impl() = default;

            resultset_impl &operator=(const resultset_impl &other) = default;
            resultset_impl &operator=(resultset_impl && other) = default;

            virtual bool is_valid() const = 0;

            virtual bool next() = 0;

            virtual row current_row() = 0;

            virtual void reset() = 0;
        };

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

        class resultset
        {
        private:
            shared_ptr<resultset_impl> impl_;
        public:

            resultset(shared_ptr<resultset_impl> impl);

            resultset(const resultset &other);
            resultset(resultset &&other);
            virtual ~resultset();

            resultset &operator=(const resultset &other);
            resultset &operator=(resultset && other);

            typedef resultset_iterator iterator;

            bool is_valid() const;

            iterator begin();

            iterator end();

            row current_row();

            bool next();

            row operator*();

            void reset();
        };

        class sqlite3_resultset : public resultset_impl
        {
            friend class select_query;
            friend class row;
            friend class sqldb;
            friend class resultset_iterator;
        private:
            sqlite3_stmt *stmt_;

            sqlite3_db *db_;

            int status_;

        public:
            sqlite3_resultset(sqlite3_db *db, sqlite3_stmt *stmt);

            sqlite3_resultset(const sqlite3_resultset &other);
            sqlite3_resultset(sqlite3_resultset &&other);
            virtual ~sqlite3_resultset();

            sqlite3_resultset &operator=(const sqlite3_resultset &other);
            sqlite3_resultset &operator=(sqlite3_resultset && other);

            bool is_valid() const;

            row current_row();

            void reset();

            bool next();
        };
    }


}

#endif

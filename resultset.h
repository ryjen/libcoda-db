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

        class resultset_iterator : public std::iterator<std::input_iterator_tag, row>
        {
        private:
            resultset *rs_;
            int pos_;
            row value_;
        public:

            resultset_iterator(resultset *rs, int position);

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
            friend class select_query;
            friend class row;
            friend class sqldb;
            friend class resultset_iterator;
        public:
            typedef resultset_iterator iterator;
        private:
            sqlite3_stmt *stmt_;

            resultset(sqlite3_stmt *stmt);

            int status_;

            int step();
        public:
            iterator begin();

            iterator end();

            bool is_valid();

            bool has_more();

            row operator*();

            bool next();
        };
    }


}

#endif

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
            resultset *m_rs;
            int m_pos;
            row m_value;
        public:

            resultset_iterator(resultset* rs, int position);

            reference operator*();

            const reference operator*() const;

            pointer operator->();

            const pointer operator->() const;

            resultset_iterator& operator++();

            resultset_iterator operator++(int);

            resultset_iterator operator+(difference_type n);

            resultset_iterator &operator+=(difference_type n);

            bool operator==(const resultset_iterator& other) const;

            bool operator!=(const resultset_iterator& other) const;

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
        public:
            typedef resultset_iterator iterator;
        private:
            sqlite3_stmt *m_stmt;

            resultset(sqlite3_stmt *stmt);

            int m_status;
        public:
            iterator begin();

            iterator end();

            int status();

            bool has_more();

            int step();
        };
    }


}

#endif

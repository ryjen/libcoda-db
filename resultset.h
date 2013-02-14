/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_RESULTSET_H_
#define _ARG3_DB_RESULTSET_H_

#include "row.h"
#include "database_exception.h"
#include <memory>

namespace arg3
{
    namespace db
    {
        class resultset;

        class resultset_iterator //An STL iterator for CRowset
        {
            friend class resultset;
        private:

            //Constructors / Destructors
            resultset_iterator(resultset *rset, int position);

        public:
            //Typedefs
            typedef resultset_iterator         self_type;
            typedef std::input_iterator_tag iterator_category;
            typedef row                    value_type;
            typedef row                   *pointer;
            typedef const row             *const_pointer;
            typedef row                   &reference;
            typedef const row             &const_reference;
            typedef size_t                  size_type;
            typedef ptrdiff_t               difference_type;

            //Methods
            reference operator*();

            pointer operator->();

            self_type &operator++();

            self_type operator++(int);

            self_type operator+(difference_type n);

            self_type &operator+=(difference_type n);

            bool operator==(const self_type &other) const;

            bool operator!=(const self_type &other) const;

            bool operator<(const self_type &other) const;

            bool operator<=(const self_type &other) const;

            bool operator>(const self_type &other) const;

            bool operator>=(const self_type &other) const;
        protected:
            resultset *m_results;
            int m_position;
            value_type m_row;
        };

        class resultset
        {
            friend class select_query;
            friend class resultset_iterator;
            friend class row;
        public:
            //Typedefs
            typedef resultset_iterator iterator;

        private:
            sqlite3_stmt *m_stmt;

            resultset(sqlite3_stmt *stmt);

            int m_status;

            void step();

        public:
            //Methods
            iterator begin();

            iterator end();

            int status();
        };
    }


}

#endif

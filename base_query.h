/*!
 * @header Base Query
 * implementation of a query
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_BASE_QUERY_H_
#define _ARG3_DB_BASE_QUERY_H_

#include <sqlite3.h>
#include "schema.h"

namespace arg3
{
    namespace db
    {
        class sqldb;

        /*!
         * abstract class
         * override to implement a query
         */
        class base_query
        {
            friend class sqldb;
        protected:
            //sqlite3 *db_;
            sqldb *db_;
            sqlite3_stmt *stmt_;
            string tableName_;

            void prepare();
        public:

            /*!
             * @param db the database to perform the query on
             * @param tableName the table to perform the query on
             */
            base_query(sqldb *db, const string &tableName);

            base_query(const base_query &other);

            base_query(base_query &&other);

            virtual ~base_query();

            base_query &operator=(const base_query &other);

            base_query &operator=(base_query &&other);

            /*!
             * should return valid T-SQL
             */
            virtual string to_string() const = 0;

            /*!
             * binds a string to a parameterized query
             */
            base_query &bind(size_t index, const string &value, int len = -1);

            /*!
             * binds an integer to a parameterized query
             */
            base_query &bind(size_t index, int value);

            /*!
             * binds a 64 bit integer to a parameterized query
             */
            base_query &bind(size_t index, long long value);

            /*!
             * binds a null value
             */
            base_query &bind(size_t index);

            /*!
             * binds a double to a parameterized query
             */
            base_query &bind(size_t index, double value);

            base_query &bind_null(size_t index);

            /*!
             * binds bytes to a parameterized query
             */
            base_query &bind_bytes(size_t index, const void *data, size_t size, void(* pFree)(void *) = SQLITE_STATIC);

        };

    }
}

#endif
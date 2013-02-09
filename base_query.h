/*!
 * implementation of a query
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_BASE_QUERY_H_
#define _ARG3_DB_BASE_QUERY_H_

#include "defines.h"
#include <sqlite3.h>

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
            sqlite3 *m_db;
            sqlite3_stmt *m_stmt;
            string m_tableName;
            column_definition m_columns;

            void prepare();

        public:

            /*!
             * @param db the database to perform the query on
             * @param tableName the table to perform the query on
             * @param columns the columns to operate with
             */
            base_query(const sqldb &db, const string &tableName, const column_definition &columns);

            /*!
             * @param db the database to perform the query on
             * @param tableName the table to perform the query on
             */
            base_query(const sqldb &db, const string &tableName);

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

            /*!
             * binds bytes to a parameterized query
             */
            base_query &bind(size_t index, const void *data, size_t size = -1, void(* pFree)(void *) = SQLITE_STATIC);

        };

    }
}

#endif
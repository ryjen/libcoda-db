/*!
 * @header Base Query
 * implementation of a query
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_BASE_QUERY_H_
#define _ARG3_DB_BASE_QUERY_H_

#include <sqlite3.h>
#include "schema.h"
#include <map>

#include "../variant/variant.h"

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

            // internal wrapper for binding types to a sql statement
            struct bind_type
            {
            public:
                bind_type() : type(-1), size(0), freeFunc(0)
                {}
                bind_type(const string &v, int len ) : value(v), type(SQLITE_TEXT), size(len)
                {}
                bind_type(int v) : value(v), type(SQLITE_INTEGER), size(sizeof(int))
                {}
                bind_type(long long v) : value(v), type(SQLITE_INTEGER), size(sizeof(long long))
                {}
                bind_type(double v) : value(v), type(SQLITE_FLOAT), size(sizeof(double))
                {}
                bind_type(const void *p, size_t s, void (*func)(void*)) : value(p, s), type(SQLITE_BLOB), size(s), freeFunc(func)
                {}

                variant value;
                int type;
                int size;
                void (*freeFunc)(void*);
            };
            //
            size_t assert_binding_index(size_t index);
        protected:
            sqldb *db_;
            sqlite3_stmt *stmt_;
            string tableName_;
            vector<bind_type> bindings_;
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

            /*!
             * binds bytes to a parameterized query
             */
            base_query &bind(size_t index, const void *data, size_t size, void(* pFree)(void *) = SQLITE_STATIC);

            base_query &bind(size_t index, int type, const variant &v);
        };

    }
}

#endif
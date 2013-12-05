/*!
 * @header Base Query
 * implementation of a query
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_BASE_QUERY_H_
#define _ARG3_DB_BASE_QUERY_H_

#include <sqlite3.h>
#include <map>
#include <sstream>
#include "schema.h"
#include "sql_value.h"
#include "statement.h"

namespace arg3
{
    namespace db
    {
        class sqldb;

        /*!
         * abstract class
         * override to implement a query
         */
        class base_query : public bindable
        {
            friend class sqldb;

            //
            size_t assert_binding_index(size_t index);
        protected:
            sqldb *db_;
            shared_ptr<statement> stmt_;
            string tableName_;
            vector<sql_value> bindings_;
            void prepare();
        public:

            /*!
             * @param db the database to perform the query on
             * @param tableName the table to perform the query on
             */
            base_query(sqldb *db, const string &tableName);

            base_query(shared_ptr<schema> schema);

            base_query(const base_query &other);

            base_query(base_query &&other);

            virtual ~base_query();

            base_query &operator=(const base_query &other);

            base_query &operator=(base_query && other);

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

            base_query &bind(size_t index, const sql_blob &value);

            base_query &bind(size_t index, const sql_null_t &value);

            base_query &bind_value(size_t index, const sql_value &v);
        };

    }


    template<typename T>
    string join_csv(vector<T> list)
    {
        ostringstream buf;

        if (list.size() > 0)
        {
            ostream_iterator<T> it(buf, ",");

            copy(list.begin(), list.end() - 1, it);

            buf << *(list.end() - 1);
        }

        return buf.str();
    }

    string join_csv(std::string::value_type c, size_t count);

}


namespace std
{
    string to_string(const arg3::db::base_query &query);
}

#endif
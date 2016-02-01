/*!
 * @header Base Query
 * implementation of a query
 * @copyright ryan jennings (arg3.com), 2013
 */
#ifndef ARG3_DB_QUERY_H
#define ARG3_DB_QUERY_H

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
        class query : public bindable
        {
            friend class sqldb;
            /*!
            * ensures that the binding storage array is large enough
            * @param index the parameter index for binding
            * @returns the zero-based index suitable for the storage array
            * @throws invalid_argument if there is no specifier for the argument
            */
            size_t assert_binding_index(size_t index);

           protected:
            sqldb *db_;
            std::shared_ptr<statement> stmt_;
            std::vector<sql_value> bindings_;
            void prepare(const std::string &sql);

           public:
            /*!
             * @param db the database to perform the query on
             * @param tableName the table to perform the query on
             */
            query(sqldb *db);

            /*!
             * @param other the other query to copy from
             */
            query(const query &other);

            /*!
             * @param other the query being moved
             */
            query(query &&other);

            virtual ~query();

            virtual void reset();

            sqldb *db() const;

            /*!
             * @param other the other query being copied from
             */
            query &operator=(const query &other);

            /*!
             * @param other the query being moved
             */
            query &operator=(query &&other);

            /*!
             * binds a string to a parameterized query
             */
            query &bind(size_t index, const std::string &value, int len = -1);

            /*!
             * binds a wide character string to a parameterized query
             */
            query &bind(size_t index, const std::wstring &value, int len = -1);

            /*!
             * binds an integer to a parameterized query
             */
            query &bind(size_t index, int value);

            /*!
             * binds a 64 bit integer to a parameterized query
             */
            query &bind(size_t index, long long value);

            /*!
             * binds a null value
             */
            query &bind(size_t index);

            /*!
             * binds a double to a parameterized query
             */
            query &bind(size_t index, double value);

            /*!
             * binds a sql_blob type to a parameterized query
             */
            query &bind(size_t index, const sql_blob &value);

            /*!
             * binds a null type to a parameterized query
             */
            query &bind(size_t index, const sql_null_t &value);

            /*!
             * binds an arbitrary sql value to a parameterized query
             */
            query &bind_value(size_t index, const sql_value &v);

            /*!
             * returns the last error the query encountered, if any
             */
            std::string last_error();

            bool is_valid() const;
        };
    }

    /*!
     * utility method used in creating sql
     */
    template <typename T>
    std::string join_csv(const std::vector<T> &list)
    {
        ostringstream buf;

        if (list.size() > 0) {
            ostream_iterator<T> it(buf, ",");

            copy(list.begin(), list.end() - 1, it);

            buf << *(list.end() - 1);
        }

        return buf.str();
    }

    /*!
     * utility method used in creating sql
     */
    std::string join_params(const vector<std::string> &columns, bool update);
}

#endif

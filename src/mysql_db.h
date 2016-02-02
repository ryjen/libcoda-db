/*!
 * @file mysql_db.h
 * a mysql specific database
 */
#ifndef ARG3_DB_MYSQL_SQLDB_H
#define ARG3_DB_MYSQL_SQLDB_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBMYSQLCLIENT

#include <mysql/mysql.h>
#include "sqldb.h"

namespace arg3
{
    namespace db
    {
        /*!
         * a mysql specific implementation of a database
         */
        class mysql_db : public sqldb
        {
            friend class base_query;
            friend class mysql_statement;
            friend class mysql_resultset;
            friend class mysql_cached_resultset;

           protected:
            std::shared_ptr<MYSQL> db_;

           public:
            /*!
             * default constructor takes a uri to connect to
             * @param connInfo the uri connection info
             */
            mysql_db(const uri &connInfo);

            /* boilerplate */
            mysql_db(const mysql_db &other);
            mysql_db(mysql_db &&other);
            mysql_db &operator=(const mysql_db &other);
            mysql_db &operator=(mysql_db &&other);
            virtual ~mysql_db();

            /*!
             * @return true if the database is open and connected
             */
            bool is_open() const;

            /*!
             * opens the database
             */
            void open();

            /*!
             * closes the database connection
             */
            void close();

            /*!
             * @param tablename the name of the table to query
             * @param column the vector to store column definitions found
             */
            void query_schema(const string &tablename, std::vector<column_definition> &columns);

            /*!
             * @return the last insert id for any query
             */
            long long last_insert_id() const;

            /*!
             * @return the last number of changes for any query
             */
            int last_number_of_changes() const;

            /*!
             * @return the error message of the last failure
             */
            string last_error() const;

            /*!
             * performs a query that returns results
             * @return the results of the query
             * @return cache set to true to remove dependency on an open database (default: false)
             */
            resultset execute(const string &sql, bool cache = false);

            /*!
             * @return a database specific implementation of a statement
             */
            shared_ptr<statement> create_statement();
        };
    }
}

#endif

#endif

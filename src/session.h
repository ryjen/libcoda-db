/*!
 * @file session.h
 * a sql database
 * @copyright ryan jennings (arg3.com), 2013
 */
#ifndef ARG3_DB_SESSION_H
#define ARG3_DB_SESSION_H

#include "sql_value.h"
#include "schema_factory.h"
#include "statement.h"
#include "transaction.h"
#include "uri.h"

namespace arg3
{
    namespace db
    {
        /*!
         *  abstract class for a specific implementation of a database
         */
        class session : public std::enable_shared_from_this<session>
        {
           public:
            typedef resultset resultset_type;
            typedef statement statement_type;
            typedef transaction transaction_type;

           protected:
            session();

            /*!
             * @param info  the connection info
             */
            session(const uri &info);

            /*!
             * sets the connection info for this database
             * @param value the uri value
             */
            void set_connection_info(const uri &value);

           public:
            /* default boilerplate */
            session(const session &other) = delete;
            session(session &&other) = default;
            session &operator=(const session &other) = delete;
            session &operator=(session &&other) = default;
            virtual ~session() = default;

            /*!
             * tests if a connection to the database is open
             * @return true if the connection is open
             */
            virtual bool is_open() const = 0;

            /*!
             * opens a connection to the database
             */
            virtual void open() = 0;

            /*!
             * closes the connection to the database
             */
            virtual void close() = 0;

            /*!
             * gets the last insert id from any statement
             * @return the last insert id or zero
             */
            virtual long long last_insert_id() const = 0;

            /*!
             * gets the last number of modified records for any statement
             * @return the last number of changes or zero
             */
            virtual int last_number_of_changes() const = 0;

            /*!
             * executes a sql statement that returns results
             * @param  sql   the sql string to execute
             * @return       the results of the query
             */
            virtual resultset_type query(const std::string &sql) = 0;

            /*!
             * executes a sql statement that does not return results
             * @param  sql the sql string to execute
             * @return     true if successful
             */
            virtual bool execute(const std::string &sql) = 0;

            /*!
             * @return a statement for this database
             */
            virtual std::shared_ptr<statement_type> create_statement() = 0;

            virtual transaction_type create_transaction() = 0;

            /*!
             * gets the last error for any statement
             * @return the last error or an empty string
             */
            virtual std::string last_error() const = 0;

            /*!
             * get the schemas for this database
             * @return a schema factory object
             */
            std::shared_ptr<schema> get_schema(const std::string &tableName);

            /*!
             * clears the cache for a schema
             * @param tableName the table to clear
             */
            void clear_schema(const std::string &tableName);

            /*!
             * queries the database for a tables column definitions
             * @param tablename the tablename
             * @param columns   the collection of columns to store the results
             */
            virtual void query_schema(const std::string &tablename, std::vector<column_definition> &columns) = 0;

            /*!
             * gets the connection info for this database
             * @return the connection info uri
             */
            uri connection_info() const;

            /*!
             * generates database specific insert sql
             * @param  schema  the schema to insert to
             * @param  columns the columns to insert
             * @return         the sql string
             */
            virtual std::string insert_sql(const std::shared_ptr<schema> &schema, const std::vector<std::string> &columns) const;

           private:
            uri connectionInfo_;

           protected:
            schema_factory schema_factory_;
        };
    }
}

#endif

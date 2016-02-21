/*!
 * @file sqldb.h
 * a sql database
 * @copyright ryan jennings (arg3.com), 2013
 */
#ifndef ARG3_DB_SQLDB_H
#define ARG3_DB_SQLDB_H

#include <string>
#include <sstream>
#include <type_traits>
#include "sql_value.h"
#include "schema_factory.h"
#include "statement.h"

namespace arg3
{
    namespace db
    {
        class resultset;

        typedef struct uri_type uri;

        namespace log
        {
            /*! levels of logging */
            typedef enum {
                /*! logging is disabled */
                None = 0,
                /*! only error messages will be logged */
                Error = 1,
                /*! warnings and errors will be logged */
                Warn = 2,
                /*! info, warning, and error messages will be logged */
                Info = 3,
                /*! debug, info, warning and error messages will be logged */
                Debug = 4,
                /*! trace, debug, info, warning and error messages will be logged */
                Trace = 5
            } level;

            /*!
             * sets the logging level
             * @param level the level to set
             */
            void set_level(level level);
        }

        /*! small utility to parse a uri */
        struct uri_type {
           public:
            uri_type()
            {
            }

            /*!
             * @param url the url to parse
             */
            uri_type(const std::string &url)
            {
                parse(url);
            }

            /*!
             * decomposes a uri into its parts
             * @param url the url to parse
             */
            void parse(const std::string &url);

            operator std::string() const
            {
                return value;
            }

            std::string protocol, user, password, host, port, path, query, value;
        };

        /*!
         *  abstract class for a specific implementation of a database
         */
        class sqldb
        {
           public:
            typedef resultset resultset_type;
            typedef statement statement_type;

            /*!
             * parses a uri and returns a database object
             * @param value   the uri string to parse
             */
            static std::shared_ptr<sqldb> from_uri(const std::string &value);

            sqldb();

            /*!
             * @param info  the connection info
             */
            sqldb(const uri &info);

            /* default boilerplate */
            sqldb(const sqldb &other) = default;
            sqldb(sqldb &&other) = default;
            sqldb &operator=(const sqldb &other) = default;
            sqldb &operator=(sqldb &&other) = default;
            virtual ~sqldb() = default;

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
             * executes a sql statement
             * @param  sql   the sql string to execute
             * @param  cache true if the results should be independent from the database connection
             * @return       the results of the query
             */
            virtual resultset_type execute(const std::string &sql) = 0;

            /*!
             * @return a statement for this database
             */
            virtual std::shared_ptr<statement_type> create_statement() = 0;

            /*!
             * gets the last error for any statement
             * @return the last error or an empty string
             */
            virtual std::string last_error() const = 0;

            /*!
             * get the schemas for this database
             * @return a schema factory object
             */
            schema_factory *schemas();

            const schema_factory *schemas() const;

            /*!
             * queries the database for a tables column definitions
             * @param tablename the tablename
             * @param columns   the collection of columns to store the results
             */
            virtual void query_schema(const std::string &tablename, std::vector<column_definition> &columns);

            /*!
             * gets the connection info for this database
             * @return the connection info uri
             */
            uri connection_info() const;

            void set_connection_info(const uri &value);

            virtual std::string insert_sql(const std::shared_ptr<schema> &schema, const std::vector<std::string> &columns) const;

           private:
            uri connectionInfo_;

           protected:
            schema_factory schema_factory_;
        };
    }
}

#endif

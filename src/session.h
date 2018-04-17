/*!
 * @file session.h
 * a sql database
 * @copyright ryan jennings (ryan-jennings.net), 2013
 */
#ifndef CODA_DB_SESSION_H
#define CODA_DB_SESSION_H

#include <memory>
#include <vector>
#include "schema_factory.h"
#include "uri.h"

namespace coda {
    namespace db {
        struct column_definition;

        class schema;

        class transaction;

        class transaction_impl;

        class statement;

        class resultset;

        class resultset_impl;

        class session_impl {
        public:
            typedef statement statement_type;
            typedef transaction_impl transaction_type;
            typedef resultset_impl resultset_type;

            /*!
             * default constructor expects a uri
             */
            session_impl(const uri &connectionInfo);

            /*!
             * gets the connection info
             * @return the uri
             */
            uri connection_info() const;

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
             * gets the last error for any statement
             * @return the last error or an empty string
             */
            virtual std::string last_error() const = 0;

            /*!
             * executes a sql statement that returns results
             * @param  sql   the sql string to execute
             * @return       the results of the query
             */
            virtual std::shared_ptr<resultset_type> query(const std::string &sql) = 0;

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

            /*!
             * return the created transaction
             */
            virtual std::shared_ptr<transaction_type> create_transaction() const = 0;

            /*!
             * query the schema for a table
             * @param tablename the table to query
             * @param columns   an array to put columns found
             */
            virtual std::vector<column_definition> get_columns_for_schema(const std::string &dbName,
                                                                          const std::string &tablename) = 0;

            /*!
             * binds a null value
             */
            virtual std::string bind_param(size_t index) const = 0;

            virtual int features() const;

        private:
            uri connectionInfo_;
        };

        /*!
         *  abstract class for a specific implementation of a database
         */
        class session : public std::enable_shared_from_this<session> {
            friend struct sqldb;

        public:
            typedef resultset resultset_type;
            typedef statement statement_type;
            typedef transaction transaction_type;

            session(const std::shared_ptr<session_impl> &impl);

            /* default boilerplate */
            session(const session &other);

            session(session &&other);

            session &operator=(const session &other);

            session &operator=(session &&other);

            virtual ~session();

            /*!
             * tests if a connection to the database is open
             * @return true if the connection is open
             */
            bool is_open() const;

            /*!
             * opens a connection to the database
             */
            void open();

            /*!
             * closes the connection to the database
             */
            void close();

            /*!
             * gets the last insert id from any statement
             * @return the last insert id or zero
             */
            long long last_insert_id() const;

            /*!
             * gets the last number of modified records for any statement
             * @return the last number of changes or zero
             */
            int last_number_of_changes() const;

            /*!
             * @return a statement for this database
             */
            std::shared_ptr<statement_type> create_statement();

            /*!
             * creates a transaction, but won't start it yet
             * @return the created transaction object
             */
            transaction_type create_transaction();

            /*!
             * creates a transaction and starts it
             * @return the created transaction object
             */
            transaction_type start_transaction();

            /*!
             * gets the last error for any statement
             * @return the last error or an empty string
             */
            std::string last_error() const;

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
             * gets the connection info for this database
             * @return the connection info uri
             */
            uri connection_info() const;

            /*!
             * gets the implementation
             */
            std::shared_ptr<session_impl> impl() const;

            /*!
             * gets the implementation casted to a subtype
             */
            template<typename T>
            std::shared_ptr<T> impl() const {
                return std::dynamic_pointer_cast<T>(impl());
            }

            /*!
             * utility method used in creating sql
             */
            std::string join_params(const std::vector<std::string> &columns, const std::string &op = "") const;

        private:
            std::shared_ptr<session_impl> impl_;

        protected:
            friend class insert_query;

            friend class schema;

            /*!
             * queries the database for a tables column definitions
             * @param tablename the tablename
             * @param columns   the collection of columns to store the results
             */
            std::vector<column_definition> get_columns_for_schema(const std::string &tablename);

            schema_factory schema_factory_;

        public:
            typedef enum {
                FEATURE_RETURNING = (1 << 0),
                FEATURE_FULL_OUTER_JOIN = (1 << 1),
                FEATURE_RIGHT_JOIN = (1 << 2),
                FEATURE_NAMED_PARAMS = (1 << 3)
            } feature_type;

            bool has_feature(feature_type feature) const;
        };
    }
}

#endif

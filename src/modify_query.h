/*!
 * @file modify_query.h
 * representation of sql queries that modify data
 * @copyright ryan jennings (arg3.com), 2013
 */
#ifndef ARG3_DB_MODIFY_QUERY_H
#define ARG3_DB_MODIFY_QUERY_H

#include "query.h"
#include "sqldb.h"
#include "where_clause.h"

namespace arg3
{
    namespace db
    {
        /*!
         * a query to modify a table
         */
        class modify_query : public query
        {
           public:
            /*!
             * query flags
             */
            /*! perform subsequent executes in batches */
            constexpr static const int Batch = (1 << 0);

            /*!
             * @param db the database in use
             */
            modify_query(sqldb *db);

            /*!
             * @param schema the schema to modify
             */
            modify_query(const std::shared_ptr<schema> &schema);

            /* boilerplate */
            modify_query(const modify_query &other);
            modify_query(modify_query &&other);
            virtual ~modify_query();
            modify_query &operator=(const modify_query &other);
            modify_query &operator=(modify_query &&other);

            /*!
             * @return a string/sql representation of this query
             */
            virtual std::string to_string() const = 0;

            /*!
             * sets flags for this query (@see query flags)
             */
            modify_query &set_flags(int value);

            /*!
             * executes this query using a replace statement
             * @return the last number of changes made by this query
             */
            virtual int execute();

            /*!
             * @return the last number of changes made by this query
             */
            int last_number_of_changes() const;

           protected:
            int flags_;
            int numChanges_;
        };

        /*!
         * a query to insert to a table
         */
        class insert_query : public modify_query
        {
           public:
            using modify_query::modify_query;

            /*!
             * @param db the database to modify
             * @param tableName the table to modify
             * @param columns the columns to modify
             */
            insert_query(sqldb *db, const std::string &tableName);

            /*!
             * @param db the database to modify
             * @param columns the columns to modify
             */
            insert_query(sqldb *db, const std::string &tableName, const std::vector<std::string> &columns);

            /*!
             * @param schema the schema to modify
             * @param column the specific columns to modify in the schema
             */
            insert_query(const std::shared_ptr<schema> &schema, const std::vector<std::string> &columns);

            /* boilerplate */
            insert_query(const insert_query &other);
            insert_query(insert_query &&other);
            virtual ~insert_query();
            insert_query &operator=(const insert_query &other);
            insert_query &operator=(insert_query &&other);

            /*!
             * @return the id column of the last insert
             */
            long long last_insert_id() const;

            /*!
             * @return the sql/string representation of this query
             */
            std::string to_string() const;

            /*!
             * get the columns being modified
             * @return the list of columns
             */
            std::vector<std::string> columns() const;

            /*!
             * set the columns to modify
             * @param  value the list of column names
             * @return       a reference to this instance
             */
            insert_query &columns(const std::vector<std::string> &value);

            /*!
             * set the table to insert into
             * @see modify_query::table_name
             * @param  tableName the table name
             * @return           a reference to this instance
             */
            insert_query &into(const std::string &tableName);

            /*!
             * get the table name being inserted into
             * @see modify_query::table_name
             * @return the table name
             */
            std::string into() const;

            template <typename T, typename... List>
            insert_query &values(const T &value, const List &... argv)
            {
                bind_list(1, value, argv...);
                return *this;
            }

            /*!
             * executes the insert query
             * @return the number of records inserted
             */
            int execute();

            bool is_valid() const;

           private:
            long long lastId_;
            std::vector<std::string> columns_;
            std::string tableName_;
        };

        /*!
         * a query to update a table
         */
        class update_query : public modify_query
        {
           public:
            using modify_query::modify_query;

            /*!
             * @param db the database to modify
             * @param tableName the table to modify
             * @param columns the columns to modify
             */
            update_query(sqldb *db, const std::string &tableName);

            /*!
             * @param db the database to modify
             * @param columns the columns to modify
             */
            update_query(sqldb *db, const std::string &tableName, const std::vector<std::string> &columns);

            /*!
             * @param schema the schema to modify
             * @param column the specific columns to modify in the schema
             */
            update_query(const std::shared_ptr<schema> &schema, const std::vector<std::string> &columns);

            /* boilerplate */
            update_query(const update_query &other);
            update_query(update_query &&other);
            virtual ~update_query();
            update_query &operator=(const update_query &other);
            update_query &operator=(update_query &&other);

            /*!
             * get the columns being modified
             * @return the list of columns
             */
            std::vector<std::string> columns() const;

            /*!
             * set the columns to modify
             * @param  value the list of column names
             * @return       a reference to this instance
             */
            update_query &columns(const std::vector<std::string> &value);

            /*!
             * set the table to insert into
             * @see modify_query::table_name
             * @param  tableName the table name
             * @return           a reference to this instance
             */
            update_query &table(const std::string &tableName);

            /*!
             * get the table name being inserted into
             * @see modify_query::table_name
             * @return the table name
             */
            std::string table() const;

            /*!
             * sets the where clause for the update query
             * @param value the where clause to set
             */
            update_query &where(const where_clause &value);

            /*!
             * sets the where clause and binds a list of values
             * @param value the where clause to set
             * @param args a variadic list of indexed bind values
             * @return a reference to this instance
             */
            template <typename... List>
            update_query &where(const where_clause &value, const List &... args)
            {
                where(value);
                bind_all(args...);
                return *this;
            }

            /*!
             * @param value the where sql/string to set
             */
            where_clause &where(const std::string &value);

            /*!
             * sets the where clause and binds a list of values
             * @param value the sql where clause string
             * @param args the variadic list of indexed bind values
             * @return a reference to this instance
             */
            template <typename... List>
            update_query &where(const std::string &value, const List &... args)
            {
                where(value);
                bind_all(args...);
                return *this;
            }

            /*!
             * a rename of the bind_all method so it makes sense to the query language
             * @param value a value to bind
             * @param argv the variadic list of values to bind
             */
            template <typename T, typename... List>
            update_query &values(const T &value, const List &... argv)
            {
                bind_list(1, value, argv...);
                return *this;
            }

            /*!
             * @return the string/sql representation of this query
             */
            std::string to_string() const;

            bool is_valid() const;

           protected:
            where_clause where_;
            std::vector<std::string> columns_;
            std::string tableName_;
        };

        /*!
         * a query to delete from a table
         */
        class delete_query : public modify_query
        {
           public:
            using modify_query::modify_query;

            delete_query(sqldb *db, const std::string &tableName);

            delete_query(const std::shared_ptr<schema> &schema);

            /* boilerplate */
            delete_query(const delete_query &other);
            delete_query(delete_query &&other);
            virtual ~delete_query();
            delete_query &operator=(const delete_query &other);
            delete_query &operator=(delete_query &&other);

            /*!
             * set the table to insert into
             * @see modify_query::table_name
             * @param  tableName the table name
             * @return           a reference to this instance
             */
            delete_query &from(const std::string &tableName);

            /*!
             * get the table name being inserted into
             * @see modify_query::table_name
             * @return the table name
             */
            std::string from() const;

            /*!
             * @return the string/sql representation of this query
             */
            std::string to_string() const;

            /*!
             * sets the where clause for the update query
             * @param value the where clause to set
             */
            delete_query &where(const where_clause &value);

            /*!
             * sets the where clause and binds a list of values
             * @param value the where clause to set
             * @param args a variadic list of indexed bind values
             * @return a reference to this instance
             */
            template <typename... List>
            delete_query &where(const where_clause &value, const List &... args)
            {
                where(value);
                bind_all(args...);
                return *this;
            }

            /*!
             * @param value the where sql/string to set
             */
            where_clause &where(const std::string &value);

            /*!
             * sets the where clause and binds a list of values
             * @param value the sql where clause string
             * @param args the variadic list of indexed bind values
             * @return a reference to this instance
             */
            template <typename... List>
            delete_query &where(const std::string &value, const List &... args)
            {
                where(value);
                bind_all(args...);
                return *this;
            }

            bool is_valid() const;

           private:
            where_clause where_;
            std::string tableName_;
        };
    }
}

#endif

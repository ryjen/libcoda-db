#ifndef ARG3_DB_INSERT_QUERY_H
#define ARG3_DB_INSERT_QUERY_H

#include "modify_query.h"

namespace arg3
{
    namespace db
    {
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
            insert_query(const std::shared_ptr<arg3::db::session> &session, const std::string &tableName);

            /*!
             * @param db the database to modify
             * @param columns the columns to modify
             */
            insert_query(const std::shared_ptr<arg3::db::session> &session, const std::string &tableName, const std::vector<std::string> &columns);

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

            /*!
             * binds values to the query.  similar to bind_all but makes more sense in a query context
             * @param value a value to bind
             * @param argv a variadic list of values to bind
             * @return a reference to this instance
             */
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

            /*!
             * tests if this query is valid
             * @return true if valid
             */
            bool is_valid() const;

           private:
            long long lastId_;
            std::vector<std::string> columns_;
            std::string tableName_;
        };
    }
}

#endif

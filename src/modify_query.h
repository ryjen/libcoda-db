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
             * @param db the database to modify
             * @param tableName the table to modify
             * @param columns the columns to modify
             */
            modify_query(sqldb *db, const std::string &tableName, const std::vector<std::string> &columns);

            /*!
             * @param db the database to modify
             * @param tableName the table to modify
             */
            modify_query(sqldb *db, const std::string &tableName);

            /*!
             * @param schema the schema to modify
             */
            modify_query(const std::shared_ptr<schema> &schema);

            /*!
             * @param schema the schema to modify
             * @param column the specific columns to modify in the schema
             */
            modify_query(const std::shared_ptr<schema> &schema, const std::vector<std::string> &columns);

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
             * @return the name of the table being modified
             */
            std::string table_name() const;

            /*!
             * sets flags for this query (@see query flags)
             */
            modify_query &set_flags(int value);

            /*!
             * sets the table name for this query
             */
            modify_query &table_name(const std::string &value);

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
            std::vector<std::string> columns_;
            std::string tableName_;
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
             * @return the id column of the last insert
             */
            long long last_insert_id() const;

            /*!
             * @return the sql/string representation of this query
             */
            std::string to_string() const;

            /*!
             * executes the insert query
             * @return the number of records inserted
             */
            int execute();

           private:
            long long lastId_;
        };

        /*!
         * a query to update a table
         */
        class update_query : public modify_query
        {
           public:
            using modify_query::modify_query;

            /*!
             * sets the where clause for the update query
             * @param value the where clause to set
             */
            update_query &where(const where_clause &value);

            /*!
             * @param value the where sql/string to set
             */
            where_clause &where(const string &value);

            /*!
             * @return the string/sql representation of this query
             */
            virtual std::string to_string() const;

           protected:
            where_clause where_;
        };
        /*!
         * a query to delete from a table
         */
        class delete_query : public update_query
        {
           public:
            using update_query::update_query;

            /*!
             * @return the string/sql representation of this query
             */
            std::string to_string() const;
        };
    }
}

#endif

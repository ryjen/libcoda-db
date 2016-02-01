/*!
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
         * a query to modify a table (replace query)
         */
        class modify_query : public query
        {
           public:
            /*! perform subsequent executes in batches */
            constexpr static const int BATCH = (1 << 0);

            modify_query(sqldb *db, const std::string &tableName, const std::vector<std::string> &columns);

            modify_query(sqldb *db, const std::string &tableName);

            modify_query(const std::shared_ptr<schema> &schema);

            modify_query(const std::shared_ptr<schema> &schema, const std::vector<std::string> &columns);

            modify_query(const modify_query &other);

            modify_query(modify_query &&other);

            virtual ~modify_query();

            modify_query &operator=(const modify_query &other);

            modify_query &operator=(modify_query &&other);

            virtual std::string to_string() const = 0;

            std::string table_name() const;

            modify_query &set_flags(int value);

            modify_query &table_name(const std::string &value);

            /*! executes this query using a replace statement
             * @return the last number of changes made by this query
             */
            virtual int execute();

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

            long long last_insert_id() const;

            std::string to_string() const;

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

            update_query &where(const where_clause &value);

            where_clause &where(const string &value);

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

            std::string to_string() const;
        };
    }
}

#endif

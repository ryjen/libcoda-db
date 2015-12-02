/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
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
         * a query to modify a table in some what (update or insert)
         */
        class modify_query : public query
        {
           public:
            /*! perform subsequent executes in batches */
            constexpr static const int BATCH = (1 << 0);

            modify_query(sqldb *db, const string &tableName, const vector<string> &columns);

            modify_query(sqldb *db, const string &tableName);

            modify_query(shared_ptr<schema> schema);

            modify_query(const modify_query &other);

            modify_query(modify_query &&other);

            virtual ~modify_query();

            modify_query &operator=(const modify_query &other);

            modify_query &operator=(modify_query &&other);

            virtual string to_string() const;

            string table_name() const;

            modify_query &set_flags(int value);

            modify_query &table_name(const string &value);

            /*! executes this query using a replace statement
             * @return the last number of changes made by this query
             */
            virtual int execute();

           protected:
            vector<string> columns_;
            string tableName_;
            int flags_;
        };

        class insert_query : public modify_query
        {
           public:
            using modify_query::modify_query;

            long long last_insert_id() const;

            string to_string() const;

            int execute();

           private:
            long long lastId_;
        };

        class update_query : public modify_query
        {
           public:
            using modify_query::modify_query;

            update_query &where(const where_clause &value);

            update_query &where(const string &value);

            virtual string to_string() const;

           protected:
            where_clause where_;
        };

        class delete_query : public update_query
        {
           public:
            using update_query::update_query;

            string to_string() const;
        };
    }
}

#endif

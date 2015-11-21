#ifndef ARG3_DB_DELETE_QUERY_H_
#define ARG3_DB_DELETE_QUERY_H_

#include "query.h"
#include "where_clause.h"

namespace arg3
{
    namespace db
    {
        /*!
         * a query to delete from a table
         */
        class delete_query : public query
        {
           public:
            delete_query(sqldb *db, const string &tableName);

            delete_query(shared_ptr<schema> schema);

            delete_query(const delete_query &other);

            delete_query(delete_query &&other);

            virtual ~delete_query();

            delete_query &operator=(const delete_query &other);

            delete_query &operator=(delete_query &&other);

            string to_string() const;

            int execute(bool batch = false);

            delete_query &where(const where_clause &value);

            delete_query &where(const string &value);

            string table_name() const;

            delete_query &table_name(const string &value);

           private:
            where_clause where_;
            string tableName_;
        };
    }
}

#endif

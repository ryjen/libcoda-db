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

            string to_string() const;
            string to_update_string(const where_clause &where) const;
            string to_insert_string() const;

            string table_name() const;

            modify_query &set_flags(int value);

            long long last_insert_id() const;

            modify_query &table_name(const string &value);

            /*! executes this query using a replace statement
             * @return the last number of changes made by this query
             */
            int execute();

            /*! executes this query using a update statement
             * @param where the WHERE portion of the update sql query
             * @return the last number of changes made by this query
             */
            int executeUpdate(const where_clause &where);

            /*! executes this query using a insert statement
             * @return the last number of changes made by this query
             */
            int executeInsert();

           private:
            vector<string> columns_;
            string tableName_;
            long long lastId_;
            int flags_;
        };
    }
}

#endif

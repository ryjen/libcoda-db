/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef ARG3_DB_MODIFY_QUERY_H
#define ARG3_DB_MODIFY_QUERY_H

#include "query.h"
#include "sqldb.h"

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
            modify_query(sqldb *db, const string &tableName, const vector<string> &columns);

            modify_query(sqldb *db, const string &tableName);

            modify_query(shared_ptr<schema> schema);

            modify_query(const modify_query &other);

            modify_query(modify_query &&other);

            virtual ~modify_query();

            modify_query &operator=(const modify_query &other);

            modify_query &operator=(modify_query &&other);

            string to_string() const;
            string to_update_string(const std::string &idColumnName) const;
            string to_insert_string() const;

            string table_name() const;

            modify_query &table_name(const string &value);

            int execute(long long *insertId = NULL, bool batch = false);
            int executeUpdate(const std::string &idColumnName, bool batch = false);
            int executeInsert(long long *insertId = NULL, bool batch = false);

           private:
            vector<string> columns_;
            string tableName_;
        };
    }
}

#endif

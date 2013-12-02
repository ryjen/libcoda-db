/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_MODIFY_QUERY_H_
#define _ARG3_DB_MODIFY_QUERY_H_

#include "base_query.h"
#include "sqldb.h"

namespace arg3
{
    namespace db
    {

        class modify_query : public base_query
        {
        public:
            modify_query(sqldb *db, const string &tableName, const vector<string> &columns);

            modify_query(sqldb *db, const string &tableName);

            modify_query(shared_ptr<schema> schema);

            modify_query(const modify_query &other);

            modify_query(modify_query &&other);

            virtual ~modify_query();

            modify_query &operator=(const modify_query &other);

            modify_query &operator=(modify_query && other);

            string to_string() const;

            bool execute(bool batch = false);
        private:
            vector<string> columns_;
        };

    }
}

#endif

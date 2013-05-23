#ifndef ARG3_DB_DELETE_QUERY_H_
#define ARG3_DB_DELETE_QUERY_H_

#include "base_query.h"
#include "where_clause.h"

namespace arg3
{
    namespace db {
        class delete_query : public base_query
        {
        public:
            delete_query(sqldb *db, const string &tableName);

            delete_query(const delete_query &other);

            delete_query(delete_query &&other);

            virtual ~delete_query();

            delete_query &operator=(const delete_query &other);

            delete_query &operator=(delete_query &&other);

            string to_string() const;

            bool execute(bool batch = false);

            delete_query &where(const where_clause &value);

            delete_query &where(const string &value);

            delete_query &limit(const string &value);

            delete_query &orderBy(const string &value);

        private:

            where_clause where_;
            string limit_;
            string orderBy_;
        };
    }
}

#endif

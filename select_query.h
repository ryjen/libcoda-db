#ifndef _ARG3_DB_SELECT_QUERY_H_
#define _ARG3_DB_SELECT_QUERY_H_

#include "base_query.h"
#include "base_record.h"
#include "resultset.h"

namespace arg3
{
    namespace db
    {
        class select_query : public base_query
        {
            friend class resultset;
        private:
            string m_where;
            string m_limit;
            string m_orderBy;
            string m_groupBy;
        public:
            select_query(const base_record &record);

            select_query(const sqldb &db, const string &tableName, const column_definition &columns);

            select_query(const sqldb &db, const string &tableName);

            void where(const string &value);

            void limit(const string &value);

            void orderBy(const string &value);

            void groupBy(const string &value);

            string to_string() const;

            resultset execute();
        };

    }
}

#endif

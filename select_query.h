#ifndef _ARG3_DB_SELECT_QUERY_H_
#define _ARG3_DB_SELECT_QUERY_H_

#include "base_query.h"
#include "resultset.h"

namespace arg3
{
    namespace db
    {   
        class where
        {
        private:
            vector<where> m_and;
            vector<where> m_or;
            string m_value;

        public:
            where();
            where(const string &value);

            string to_string() const;

            operator string();

            where &operator&&(const where &value);
            where &operator||(const where &value);

            bool empty() const;
        };

        class base_record;
        class sqldb;

        class select_query : public base_query
        {
            friend class resultset;
        private:
            where m_where;
            string m_limit;
            string m_orderBy;
            string m_groupBy;
        public:
            select_query(const base_record &record);

            select_query(const sqldb &db, const string &tableName, const column_definition &columns);

            select_query(const sqldb &db, const string &tableName);

            select_query &where(const where& value);

            select_query & limit(const string &value);

            select_query & orderBy(const string &value);

            select_query & groupBy(const string &value);

            string to_string() const;

            resultset execute();
        };

    }
}

#endif

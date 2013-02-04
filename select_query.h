#ifndef _ARG3_DB_SELECT_QUERY_H_
#define _ARG3_DB_SELECT_QUERY_H_

#include "base_query.h"
#include "base_record.h"

namespace arg3
{
	namespace db
	{

        class select_query : public base_query
        {
        private:
            string mWhere;
            string mLimit;
            string mOrderBy;
            string mGroupBy;
        public:
        	select_query(const base_record &record);

        	select_query(const sqldb &db, const string &tableName, const columnset &columns);

        	void where(const string &value) { mWhere = value; }

        	void limit(const string &value) { mLimit = value; }

        	void orderBy(const string &value) { mOrderBy = value; }

        	void groupBy(const string &value) { mGroupBy = value; }

            string to_string() const;

            rowset execute();
        };

	}
}

#endif

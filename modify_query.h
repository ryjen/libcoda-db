#ifndef _ARG3_DB_MODIFY_QUERY_H_
#define _ARG3_DB_MODIFY_QUERY_H_

#include "base_query.h"
#include "base_record.h"
#include "sqldb.h"

namespace arg3
{
	namespace db
	{
        class modify_query : public base_query
        {
        public:
        	modify_query(const base_record &record);

			modify_query(const sqldb &db, const string &tableName, const columnset &columns);

            string to_string() const;

            bool execute();
        };

	}
}

#endif

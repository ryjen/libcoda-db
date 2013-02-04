#ifndef _ARG3_DB_BASE_QUERY_H_
#define _ARG3_DB_BASE_QUERY_H_

#include "defines.h"
#include "sqldb.h"

namespace arg3
{
	namespace db
	{

        class base_query
        {
            friend class sqldb;
        protected:
            sqlite3 *mDb;
            sqlite3_stmt *mStmt;
            string mTableName;
            columnset mColumns;

            void prepare();

        public:
        	base_query(const sqldb &db, const string &tableName, const columnset &columns);

            virtual string to_string() const = 0;

            base_query &bind(size_t index, const string &value);

            base_query &bind(size_t index, int value);

            base_query &bind(size_t index, sqlite3_int64 value);

            base_query &bind(size_t index);

            base_query &bind(size_t index, double value);

            base_query &bind(size_t index, const void *data, size_t size);
        };

	}
}

#endif
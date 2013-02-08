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
            sqlite3 *m_db;
            sqlite3_stmt *m_stmt;
            string m_tableName;
            column_definition m_columns;

            void prepare();

        public:
        	base_query(const sqldb &db, const string &tableName, const column_definition &columns);

            base_query(const sqldb &db, const string &tableName);

            virtual string to_string() const = 0;

            base_query &bind(size_t index, const string &value, int len = -1);

            base_query &bind(size_t index, int value);

            base_query &bind(size_t index, long long value);

            base_query &bind(size_t index);

            base_query &bind(size_t index, double value);

            base_query &bind(size_t index, const void *data, size_t size = -1, void(* pFree)(void *) = SQLITE_STATIC);

        };

	}
}

#endif
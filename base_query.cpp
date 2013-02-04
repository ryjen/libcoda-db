#include "base_query.h"

namespace arg3
{
	namespace db
	{
	
        base_query::base_query(const sqldb &db, const string &tableName, const columnset &columns) 
         : mDb(db.mDb), mStmt(NULL), mTableName(tableName), mColumns(columns) 
         {}

        void base_query::prepare()
        {
        	if(mStmt != NULL) return;

        	string sql = to_string();

            if (sqlite3_prepare_v2(mDb, sql.c_str(), -1, &mStmt, NULL) != SQLITE_OK)
                throw database_exception();
        }

        base_query &base_query::bind(size_t index, const string &value)
        {
        	prepare();

            if (sqlite3_bind_text(mStmt, index, value.c_str(), -1, NULL) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        base_query &base_query::bind(size_t index, int value)
        {
        	prepare();

            if (sqlite3_bind_int(mStmt, index, value) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        base_query &base_query::bind(size_t index, sqlite3_int64 value)
        {
        	prepare();

            if (sqlite3_bind_int64(mStmt, index, value) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        base_query &base_query::bind(size_t index)
        {
        	prepare();

            if (sqlite3_bind_null(mStmt, index) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        base_query &base_query::bind(size_t index, double value)
        {
        	prepare();

            if (sqlite3_bind_double(mStmt, index, value) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        base_query &base_query::bind(size_t index, const void *data, size_t size)
        {
        	prepare();

            if (sqlite3_bind_blob(mStmt, index, data, size, NULL) != SQLITE_OK)
                throw database_exception();
            return *this;
        }
	}
}
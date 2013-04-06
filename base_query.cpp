/*!
 * implementation of a query
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "base_query.h"
#include "exception.h"
#include "sqldb.h"

namespace arg3
{
    namespace db
    {

        base_query::base_query(const sqldb &db, const string &tableName, const vector<string> &columns)
            : db_(db.db_), stmt_(NULL), tableName_(tableName), columns_(columns)
        {}

        base_query::base_query(const sqldb &db, const string &tableName) : db_(db.db_), stmt_(NULL), tableName_(tableName)
        {}

        void base_query::prepare()
        {
            if(stmt_ != NULL) return;

            string sql = to_string();

            if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt_, NULL) != SQLITE_OK)
                throw database_exception();
        }

        base_query &base_query::bind(size_t index, const string &value, int len)
        {
            prepare();

            if (sqlite3_bind_text(stmt_, index, value.c_str(), len, SQLITE_TRANSIENT) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        base_query &base_query::bind(size_t index, int value)
        {
            prepare();

            if (sqlite3_bind_int(stmt_, index, value) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        base_query &base_query::bind(size_t index, long long value)
        {
            prepare();

            if (sqlite3_bind_int64(stmt_, index, value) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        base_query &base_query::bind(size_t index)
        {
            prepare();

            if (sqlite3_bind_null(stmt_, index) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        base_query &base_query::bind(size_t index, double value)
        {
            prepare();

            if (sqlite3_bind_double(stmt_, index, value) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        base_query &base_query::bind_bytes(size_t index, const void *data, size_t size, void (*pFree)(void*))
        {
            prepare();

            if (sqlite3_bind_blob(stmt_, index, data, size, pFree) != SQLITE_OK)
                throw database_exception();
            return *this;
        }
    }
}
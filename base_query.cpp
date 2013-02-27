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

        base_query::base_query(const sqldb &db, const string &tableName, const vector<column_definition> &columns)
            : m_db(db.m_db), m_stmt(NULL), m_tableName(tableName), m_columns(columns)
        {}

        base_query::base_query(const sqldb &db, const string &tableName) : m_db(db.m_db), m_stmt(NULL), m_tableName(tableName)
        {}

        void base_query::prepare()
        {
            if(m_stmt != NULL) return;

            string sql = to_string();

            if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &m_stmt, NULL) != SQLITE_OK)
                throw database_exception();
        }

        base_query &base_query::bind(size_t index, const string &value, int len)
        {
            prepare();

            if (sqlite3_bind_text(m_stmt, index, value.c_str(), len, SQLITE_TRANSIENT) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        base_query &base_query::bind(size_t index, int value)
        {
            prepare();

            if (sqlite3_bind_int(m_stmt, index, value) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        base_query &base_query::bind(size_t index, long long value)
        {
            prepare();

            if (sqlite3_bind_int64(m_stmt, index, value) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        base_query &base_query::bind(size_t index)
        {
            prepare();

            if (sqlite3_bind_null(m_stmt, index) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        base_query &base_query::bind(size_t index, double value)
        {
            prepare();

            if (sqlite3_bind_double(m_stmt, index, value) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        base_query &base_query::bind_bytes(size_t index, const void *data, size_t size, void (*pFree)(void*))
        {
            prepare();

            if (sqlite3_bind_blob(m_stmt, index, data, size, pFree) != SQLITE_OK)
                throw database_exception();
            return *this;
        }
    }
}
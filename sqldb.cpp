/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "sqldb.h"
#include "base_query.h"
#include "exception.h"

namespace arg3
{
    namespace db
    {

        sqldb::sqldb(const string &name) : m_db(NULL), m_fileName(name)
        {
        }

        sqldb::sqldb(const sqldb &other) : m_db(other.m_db), m_fileName(other.m_fileName)
        {

        }

        sqldb &sqldb::operator=(const sqldb &other)
        {
            if(this != &other)
            {
                m_db = other.m_db;
                m_fileName = other.m_fileName;
            }

            return *this;
        }

        sqldb::~sqldb()
        {
        }

        void sqldb::open()
        {

            if(m_db != NULL) return;

            if (sqlite3_open(m_fileName.c_str(), &m_db) != SQLITE_OK)
                throw database_exception();
        }

        void sqldb::close()
        {
            if(m_db == NULL) return;

            sqlite3_close(m_db);
            m_db = NULL;
        }

        string sqldb::last_error() const
        {
            ostringstream buf;

            buf << sqlite3_errcode(m_db);
            buf << ": " << sqlite3_errmsg(m_db);

            return buf.str();
        }

        void sqldb::execute(const string &sql, sql_callback callback)
        {
            if(sqlite3_exec(m_db, sql.c_str(), callback, this, NULL) != SQLITE_OK)
                throw database_exception();
        }
    }
}


namespace std
{
    string to_string(const arg3::db::base_query &query)
    {
        return query.to_string();
    }
}

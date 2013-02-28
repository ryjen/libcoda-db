/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "modify_query.h"
#include "exception.h"
#include "../collections/collections.h"
#include "../strings/strings.h"

namespace arg3
{
    namespace db
    {
        modify_query::modify_query(const sqldb &db, const string &tableName,
                                   const vector<string> &columns) : base_query(db, tableName, columns)
        {}

        modify_query::modify_query(const sqldb &db, const string &tableName) : base_query(db, tableName)
        {}

        string modify_query::to_string() const
        {
            ostringstream buf;

            buf << "REPLACE INTO " << m_tableName;

            if (m_columns.size() > 0)
            {
                buf << "(";

                buf << join(m_columns);

                buf << ") VALUES (";

                buf << join('?', m_columns.size());

                buf << ")";
            }
            else
            {
                buf << " DEFAULT VALUES";
            }

            return buf.str();
        }

        bool modify_query::execute(bool batch)
        {
            prepare();

            int res = sqlite3_step(m_stmt);

            if(!batch)
            {
                if(sqlite3_finalize(m_stmt) != SQLITE_OK)
                    throw database_exception();

                m_stmt = NULL;
            }
            else
            {
                if(sqlite3_reset(m_stmt) != SQLITE_OK)
                    throw database_exception();
            }

            return res == SQLITE_DONE;
        }
    }
}
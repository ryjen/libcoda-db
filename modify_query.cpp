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

            buf << "REPLACE INTO " << tableName_;

            if (columns_.size() > 0)
            {
                buf << "(";

                buf << join(columns_);

                buf << ") VALUES (";

                buf << join('?', columns_.size());

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

            int res = sqlite3_step(stmt_);

            if (!batch)
            {
                if (sqlite3_finalize(stmt_) != SQLITE_OK)
                    throw database_exception();

                stmt_ = NULL;
            }
            else
            {
                if (sqlite3_reset(stmt_) != SQLITE_OK)
                    throw database_exception();
            }

            return res == SQLITE_DONE;
        }
    }
}
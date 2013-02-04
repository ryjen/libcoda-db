#include "modify_query.h"
#include "../collections/collections.h"
#include "../strings/strings.h"

namespace arg3
{
	namespace db
	{

        modify_query::modify_query(const base_record &record) : base_query(record.db(), record.tableName(), record.columns()) {
        }

        modify_query::modify_query(const sqldb &db, const string &tableName, 
        	const columnset &columns) : base_query(db, tableName, columns)
        {}

        string modify_query::to_string() const
        {
            ostringstream buf;

            buf << "REPLACE INTO " << mTableName;

            if (mColumns.size() > 0)
            {
                buf << "(";

                buf << join(mColumns);

                buf << ") VALUES (";

                buf << join('?', mColumns.size());

                buf << ")";
            }
            else
            {
                buf << " DEFAULT VALUES";
            }

            return buf.str();
        }

        bool modify_query::execute()
        {
        	prepare();

            int res = sqlite3_step(mStmt);

            sqlite3_finalize(mStmt);

            return res == SQLITE_OK;
        }
	}
}
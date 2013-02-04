#include "select_query.h"
#include "../collections/collections.h"

namespace arg3
{
	namespace db
	{

        select_query::select_query(const base_record &record) : base_query(record.db(), record.tableName(), record.columns())
        {}
        
        select_query::select_query(const sqldb &db, const string &tableName, 
        	const columnset &columns) : base_query(db, tableName, columns)
        {}

        string select_query::to_string() const
        {
            ostringstream buf;

            buf << "SELECT ";

            buf << (mColumns.size() == 0 ? "*" : join(mColumns));

            buf << " FROM " << mTableName;

            if (!mWhere.empty())
            {
                buf << " WHERE " << mWhere;
            }

            if (!mOrderBy.empty())
            {
                buf << " ORDER BY " << mOrderBy;
            }

            if (!mLimit.empty())
            {
                buf << " LIMIT " << mLimit;
            }

            if (!mGroupBy.empty())
            {
                buf << " GROUP BY " << mGroupBy;
            }

            return buf.str();
        }

        rowset select_query::execute()
        {
        	prepare();

            int res = sqlite3_step(mStmt);

            rowset set;

            while (res == SQLITE_ROW)
            {
                row row;

                size_t count = sqlite3_column_count(mStmt);

                for (size_t i = 0; i < count; i++)
                {
                    string name = sqlite3_column_name(mStmt, i);

                    row[name] = reinterpret_cast<const char *>(sqlite3_column_text(mStmt, i));
                }

                set.push_back(row);

                res = sqlite3_step(mStmt);
            }

            return set;
        }
	}
}
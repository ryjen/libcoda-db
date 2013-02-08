#include "select_query.h"
#include "../collections/collections.h"

namespace arg3
{
	namespace db
	{

        select_query::select_query(const base_record &record) : base_query(record.db(), record.tableName(), record.columns())
        {}
        
        select_query::select_query(const sqldb &db, const string &tableName, 
        	const column_definition &columns) : base_query(db, tableName, columns)
        {}

        select_query::select_query(const sqldb &db, const string &tableName) : base_query(db, tableName)
        {}

        void select_query::where(const string &value)
        {
            m_where = value;
        }

        void select_query::limit(const string &value)
        {
            m_limit = value;
        }

        void select_query::orderBy(const string &value)
        {
            m_orderBy = value;
        }

        void select_query::groupBy(const string &value)
        {
            m_groupBy = value;
        }


        string select_query::to_string() const
        {
            ostringstream buf;

            buf << "SELECT ";

            buf << (m_columns.size() == 0 ? "*" : join(m_columns));

            buf << " FROM " << m_tableName;

            if (!m_where.empty())
            {
                buf << " WHERE " << m_where;
            }

            if (!m_orderBy.empty())
            {
                buf << " ORDER BY " << m_orderBy;
            }

            if (!m_limit.empty())
            {
                buf << " LIMIT " << m_limit;
            }

            if (!m_groupBy.empty())
            {
                buf << " GROUP BY " << m_groupBy;
            }

            return buf.str();
        }

        resultset select_query::execute()
        {
        	prepare();

            return resultset(m_stmt);

        }
	}
}
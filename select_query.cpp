/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "base_record.h"
#include "select_query.h"
#include "../collections/collections.h"
#include "base_record.h"

namespace arg3
{
    namespace db
    {
        select_query::where_clause::where_clause() {}

        select_query::where_clause::where_clause(const string &value) : m_value(value) {}

        string select_query::where_clause::to_string() const
        {
            ostringstream buf;

            buf << m_value;

            if(m_and.size() > 0)
            {
                buf << " AND ";
                for(auto &w : m_and)
                {
                    buf << w.to_string();
                }
            }

            if(m_or.size() > 0)
            {
                buf << " OR ";
                for(auto &w : m_or)
                {
                    buf << w.to_string();
                }
            }
            return buf.str();
        }

        bool select_query::where_clause::empty() const
        {
            return m_value.empty() && m_and.empty() && m_or.empty();
        }

        select_query::where_clause::operator string()
        {
            return to_string();
        }

        select_query::where_clause &select_query::where_clause::operator&&(const select_query::where_clause &value)
        {
            m_and.push_back(value);
            return *this;
        }
        select_query::where_clause &select_query::where_clause::operator&&(const string &value)
        {
            m_and.push_back(where_clause(value));
            return *this;
        }
        select_query::where_clause &select_query::where_clause::operator||(const select_query::where_clause &value)
        {
            m_or.push_back(value);
            return *this;
        }
        select_query::where_clause &select_query::where_clause::operator||(const string &value)
        {
            m_or.push_back(where_clause(value));
            return *this;
        }
        select_query::select_query(const sqldb &db, const string &tableName,
                                   const vector<string> &columns) : base_query(db, tableName, columns)
        {}

        select_query::select_query(const sqldb &db, const string &tableName) : base_query(db, tableName)
        {}

        select_query & select_query::where(const select_query::where_clause &value)
        {
            m_where = value;

            return *this;
        }

        select_query & select_query::where(const string &value)
        {
            m_where = where_clause(value);

            return *this;
        }

        select_query& select_query::limit(const string &value)
        {
            m_limit = value;

            return *this;
        }

        select_query& select_query::orderBy(const string &value)
        {
            m_orderBy = value;
            return *this;
        }

        select_query& select_query::groupBy(const string &value)
        {
            m_groupBy = value;
            return *this;
        }


        string select_query::to_string() const
        {
            ostringstream buf;

            buf << "SELECT ";

            buf << (m_columns.size() == 0 ? "*" : join(m_columns));

            buf << " FROM " << m_tableName;

            if (!m_where.empty())
            {
                buf << " WHERE " << m_where.to_string();
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
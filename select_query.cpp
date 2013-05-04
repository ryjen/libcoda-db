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

        select_query::where_clause::where_clause(const string &value) : value_(value) {}

        select_query::where_clause::where_clause(const select_query::where_clause &other) : value_(other.value_) {}

        select_query::where_clause::where_clause(select_query::where_clause &&other) : value_(std::move(other.value_)) {}

        select_query::where_clause::~where_clause() {}

        select_query::where_clause &select_query::where_clause::operator=(const select_query::where_clause &other)
        {
            if(this != &other)
            {
                value_ = other.value_;
            }
            return *this;
        }

        select_query::where_clause &select_query::where_clause::operator=(select_query::where_clause &&other)
        {
            if(this != &other)
            {
                value_ =std::move( other.value_ );
            }
            return *this;
        }

        string select_query::where_clause::to_string() const
        {
            ostringstream buf;

            buf << value_;

            if (and_.size() > 0)
            {
                buf << " AND ";
                for (auto & w : and_)
                {
                    buf << w.to_string();
                }
            }

            if (or_.size() > 0)
            {
                buf << " OR ";
                for (auto & w : or_)
                {
                    buf << w.to_string();
                }
            }
            return buf.str();
        }

        bool select_query::where_clause::empty() const
        {
            return value_.empty() && and_.empty() && or_.empty();
        }

        select_query::where_clause::operator string()
        {
            return to_string();
        }

        select_query::where_clause &select_query::where_clause::operator&&(const select_query::where_clause &value)
        {
            and_.push_back(value);
            return *this;
        }
        select_query::where_clause &select_query::where_clause::operator&&(const string &value)
        {
            and_.push_back(where_clause(value));
            return *this;
        }
        select_query::where_clause &select_query::where_clause::operator||(const select_query::where_clause &value)
        {
            or_.push_back(value);
            return *this;
        }
        select_query::where_clause &select_query::where_clause::operator||(const string &value)
        {
            or_.push_back(where_clause(value));
            return *this;
        }
        select_query::select_query(const sqldb &db, const string &tableName,
                                   const vector<string> &columns) : base_query(db, tableName, columns)
        {}

        select_query::select_query(const sqldb &db, const string &tableName) : base_query(db, tableName)
        {}

        select_query::select_query(const select_query &other) : base_query(other), where_(other.where_),
            limit_(other.limit_), orderBy_(other.orderBy_), groupBy_(other.groupBy_)
        {
        }

        select_query::select_query(select_query &&other) : base_query(std::move(other)), where_(std::move(other.where_)),
            limit_(std::move(other.limit_)), orderBy_(std::move(other.orderBy_)), groupBy_(std::move(other.groupBy_))
        {
        }

        select_query::~select_query()
        {}

        select_query &select_query::operator=(const select_query &other)
        {
            if(this != &other)
            {
                base_query::operator=(other);
                where_ = other.where_;
                limit_ = other.limit_;
                orderBy_ = other.orderBy_;
                groupBy_ = other.groupBy_;
            }
            return *this;
        }

        select_query &select_query::operator=(select_query &&other)
        {
            if(this != &other)
            {
                base_query::operator=(std::move(other));
                where_ = std::move(other.where_);
                limit_ = std::move(other.limit_);
                orderBy_ = std::move(other.orderBy_);
                groupBy_ = std::move(other.groupBy_);
            }
            return *this;
        }

        select_query &select_query::where(const select_query::where_clause &value)
        {
            where_ = value;

            return *this;
        }

        select_query &select_query::where(const string &value)
        {
            where_ = where_clause(value);

            return *this;
        }

        select_query &select_query::limit(const string &value)
        {
            limit_ = value;

            return *this;
        }

        select_query &select_query::orderBy(const string &value)
        {
            orderBy_ = value;
            return *this;
        }

        select_query &select_query::groupBy(const string &value)
        {
            groupBy_ = value;
            return *this;
        }


        string select_query::to_string() const
        {
            ostringstream buf;

            buf << "SELECT ";

            buf << (columns_.size() == 0 ? "*" : join(columns_));

            buf << " FROM " << tableName_;

            if (!where_.empty())
            {
                buf << " WHERE " << where_.to_string();
            }

            if (!orderBy_.empty())
            {
                buf << " ORDER BY " << orderBy_;
            }

            if (!limit_.empty())
            {
                buf << " LIMIT " << limit_;
            }

            if (!groupBy_.empty())
            {
                buf << " GROUP BY " << groupBy_;
            }

            return buf.str();
        }

        resultset select_query::execute()
        {
            prepare();

            return resultset(stmt_);

        }
    }
}
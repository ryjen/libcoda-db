/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "base_record.h"
#include "select_query.h"
#include "base_record.h"

namespace arg3
{
    namespace db
    {

        select_query::select_query(sqldb *db, const string &tableName,
                                   const vector<string> &columns) : base_query(db, tableName), columns_(columns)
        {}

        select_query::select_query(sqldb *db, const string &tableName) : base_query(db, tableName), columns_()
        {}

        select_query::select_query(const select_query &other) : base_query(other), where_(other.where_),
            limit_(other.limit_), orderBy_(other.orderBy_), groupBy_(other.groupBy_), columns_(other.columns_)
        {
        }

        select_query::select_query(shared_ptr<schema> schema) : base_query(schema), columns_(schema->column_names())
        {}

        select_query::select_query(select_query &&other) : base_query(std::move(other)), where_(std::move(other.where_)),
            limit_(std::move(other.limit_)), orderBy_(std::move(other.orderBy_)), groupBy_(std::move(other.groupBy_)), columns_(std::move(other.columns_))
        {
        }

        select_query::~select_query()
        {}

        select_query &select_query::operator=(const select_query &other)
        {
            base_query::operator=(other);
            where_ = other.where_;
            limit_ = other.limit_;
            orderBy_ = other.orderBy_;
            groupBy_ = other.groupBy_;

            columns_ = other.columns_;

            return *this;
        }

        select_query &select_query::operator=(select_query && other)
        {
            base_query::operator=(std::move(other));
            where_ = std::move(other.where_);
            limit_ = std::move(other.limit_);
            orderBy_ = std::move(other.orderBy_);
            groupBy_ = std::move(other.groupBy_);

            columns_ = std::move(other.columns_);

            return *this;
        }

        vector<string> select_query::columns() const
        {
            return columns_;
        }

        string select_query::limit() const
        {
            return limit_;
        }

        string select_query::group_by() const
        {
            return groupBy_;
        }

        string select_query::order_by() const
        {
            return orderBy_;
        }

        select_query &select_query::where(const where_clause &value)
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

        select_query &select_query::order_by(const string &value)
        {
            orderBy_ = value;
            return *this;
        }

        select_query &select_query::group_by(const string &value)
        {
            groupBy_ = value;
            return *this;
        }


        string select_query::to_string() const
        {
            ostringstream buf;

            buf << "SELECT ";

            buf << (columns_.size() == 0 ? "*" : join_csv(columns_));

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

        int select_query::count()
        {
            auto cols(columns_);

            columns_ = { "count(*)" };

            prepare();

            int value = execute_scalar<int>();

            columns_ = cols;

            return value;
        }

        resultset select_query::execute()
        {
            prepare();

            return stmt_->results();

        }

        void select_query::reset()
        {
            base_query::reset();
            stmt_ = nullptr;
        }
    }
}
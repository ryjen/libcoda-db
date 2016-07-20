/*!
 * @copyright ryan jennings (arg3.com), 2013
 */
#include "select_query.h"
#include "schema.h"
#include "statement.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        select_query::select_query(const std::shared_ptr<arg3::db::session> &session) : query(session)
        {
        }
        select_query::select_query(const std::shared_ptr<arg3::db::session> &session, const vector<string> &columns)
            : query(session), columns_(columns)
        {
        }
        select_query::select_query(const std::shared_ptr<arg3::db::session> &session, const vector<string> &columns, const string &tableName)
            : query(session), columns_(columns), tableName_(tableName)
        {
        }

        select_query::select_query(const select_query &other)
            : query(other),
              where_(other.where_),
              limit_(other.limit_),
              orderBy_(other.orderBy_),
              groupBy_(other.groupBy_),
              columns_(other.columns_),
              tableName_(other.tableName_)
        {
        }

        select_query::select_query(const shared_ptr<schema> &schema) : select_query(schema->get_session(), schema->column_names())
        {
            tableName_ = schema->table_name();
        }

        select_query::select_query(select_query &&other)
            : query(std::move(other)),
              where_(std::move(other.where_)),
              limit_(std::move(other.limit_)),
              orderBy_(std::move(other.orderBy_)),
              groupBy_(std::move(other.groupBy_)),
              columns_(std::move(other.columns_)),
              tableName_(std::move(other.tableName_))
        {
        }

        select_query::~select_query()
        {
        }
        select_query &select_query::operator=(const select_query &other)
        {
            query::operator=(other);
            where_ = other.where_;
            limit_ = other.limit_;
            orderBy_ = other.orderBy_;
            groupBy_ = other.groupBy_;
            columns_ = other.columns_;
            tableName_ = other.tableName_;

            return *this;
        }

        select_query &select_query::operator=(select_query &&other)
        {
            query::operator=(std::move(other));
            where_ = std::move(other.where_);
            limit_ = std::move(other.limit_);
            orderBy_ = std::move(other.orderBy_);
            groupBy_ = std::move(other.groupBy_);
            columns_ = std::move(other.columns_);
            tableName_ = std::move(other.tableName_);

            return *this;
        }

        select_query &select_query::from(const string &value)
        {
            tableName_ = value;
            return *this;
        }

        string select_query::from() const
        {
            return tableName_;
        }

        vector<string> select_query::columns() const
        {
            return columns_;
        }

        select_query &select_query::columns(const vector<string> &value)
        {
            columns_ = value;
            return *this;
        }

        string select_query::table_name() const
        {
            return tableName_;
        }

        select_query &select_query::table_name(const string &value)
        {
            tableName_ = value;
            return *this;
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

        where_clause select_query::where() const
        {
            return where_;
        }

        where_clause &select_query::where(const string &value)
        {
            where_ = where_clause(value);
            return where_;
        }

        select_query &select_query::where(const where_clause &value)
        {
            where_ = value;
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

        join_clause &select_query::join(const string &tableName, join::type type)
        {
            join_.emplace_back(tableName, type);
            return join_.back();
        }

        select_query &select_query::join(const join_clause &value)
        {
            join_.push_back(value);
            return *this;
        }

        select_query &select_query::union_with(const select_query &query, union_op::type type)
        {
            union_ = make_shared<union_operator>(query, type);
            return *this;
        }

        string select_query::to_string() const
        {
            ostringstream buf;

            buf << "SELECT ";

            buf << (columns_.size() == 0 ? "*" : join_csv(columns_));

            buf << " FROM " << tableName_;

            if (!join_.empty()) {
                for (auto &join : join_) {
                    buf << join;
                }
            }

            if (!where_.empty()) {
                buf << " WHERE " << where_.to_string();
            }

            if (!orderBy_.empty()) {
                buf << " ORDER BY " << orderBy_;
            }

            if (!limit_.empty()) {
                buf << " LIMIT " << limit_;
            }

            if (!groupBy_.empty()) {
                buf << " GROUP BY " << groupBy_;
            }

            if (union_) {
                buf << " UNION ";
                if (union_->type == union_op::all) {
                    buf << "ALL ";
                }
                buf << union_->query;
            } else {
                buf << ";";
            }
            return buf.str();
        }

        long long select_query::count()
        {
            auto cols(columns_);

            // sucks to backup the columns just for this
            // query, but I don't see an easy way right now
            // TODO: improve
            try {
                columns_ = {"COUNT(*)"};

                prepare(to_string());

                long long value = execute_scalar<long long>();

                columns_ = cols;

                return value;
            } catch (...) {
                // make sure we don't leave this query in a temp state
                columns_ = cols;

                return -1;
            }
        }

        resultset select_query::execute()
        {
            prepare(to_string());

            return stmt_->results();
        }

        void select_query::execute(const std::function<void(const resultset &rs)> &funk)
        {
            prepare(to_string());

            auto rs = stmt_->results();

            funk(rs);
        }

        void select_query::reset()
        {
            query::reset();
            stmt_ = nullptr;
        }

        std::ostream &operator<<(std::ostream &out, const select_query &other)
        {
            out << other.to_string();
            return out;
        }
    }
}

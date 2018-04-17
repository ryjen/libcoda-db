/*!
 * @copyright ryan jennings (ryan-jennings.net), 2013
 */

#include "select_query.h"
#include "schema.h"
#include "statement.h"

using namespace std;

namespace coda
{
    namespace db
    {
        select_query::select_query(const std::shared_ptr<coda::db::session> &session)
            : query(session), where_(session->impl(), this)
        {
        }
        select_query::select_query(const std::shared_ptr<coda::db::session> &session, const vector<string> &columns)
            : query(session), where_(session->impl(), this), columns_(columns)
        {
        }
        select_query::select_query(const std::shared_ptr<coda::db::session> &session, const vector<string> &columns,
                                   const string &tableName)
            : query(session), where_(session->impl(), this), columns_(columns), tableName_(tableName)
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

        select_query::select_query(const shared_ptr<schema> &schema)
            : select_query(schema->get_session(), schema->column_names())
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
            set_modified();
            return *this;
        }

        select_query &select_query::from(const string &value, const string &alias)
        {
            tableName_ = value + " " + alias;
            set_modified();
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
            set_modified();
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

        where_builder &select_query::where()
        {
            return where_;
        }
        where_builder &select_query::where(const sql_operator &value)
        {
            where_.reset(value);
            set_modified();
            return where_;
        }

#ifdef ENABLE_PARAMETER_MAPPING
        select_query &select_query::where(const where_clause &value)
        {
            where_.where_clause::reset(value);
            set_modified();
            return *this;
        }

        where_builder &select_query::where(const std::string &sql)
        {
            where_.where_clause::reset(sql);
            set_modified();
            return where_;
        }
#endif

        select_query &select_query::limit(const string &value)
        {
            limit_ = value;
            set_modified();
            return *this;
        }

        select_query &select_query::order_by(const string &value)
        {
            orderBy_ = value;
            set_modified();
            return *this;
        }

        select_query &select_query::group_by(const string &value)
        {
            groupBy_ = value;
            set_modified();
            return *this;
        }

        join_clause &select_query::join(const string &tableName, join::type type)
        {
            join_.emplace_back(tableName, type);
            set_modified();
            return join_.back();
        }

        join_clause &select_query::join(const string &tableName, const string &alias, join::type type)
        {
            join_.emplace_back(tableName, alias, type);
            set_modified();
            return join_.back();
        }
        select_query &select_query::join(const join_clause &value)
        {
            join_.push_back(value);
            set_modified();
            return *this;
        }

        select_query &select_query::union_with(const select_query &query, union_op::type type)
        {
            union_ = make_shared<union_operator>(query, type);
            set_modified();
            return *this;
        }

        string select_query::generate_sql() const
        {
            string buf;

            buf += "SELECT ";

            buf += (columns_.size() == 0 ? "*" : helper::join_csv(columns_));

            buf += " FROM ";

            buf += tableName_;

            if (!join_.empty()) {
                for (auto &join : join_) {
                    buf += join.to_sql();
                }
            }

            if (!where_.empty()) {
                buf += " WHERE ";
                buf += where_.to_sql();
            }

            if (!orderBy_.empty()) {
                buf += " ORDER BY ";
                buf += orderBy_;
            }

            if (!limit_.empty()) {
                buf += " LIMIT ";
                buf += limit_;
            }

            if (!groupBy_.empty()) {
                buf += " GROUP BY ";
                buf += groupBy_;
            }

            if (union_) {
                buf += " UNION ";
                if (union_->type == union_op::all) {
                    buf += "ALL ";
                }
                buf += union_->query.to_sql();
            } else {
                buf += ";";
            }
            return buf;
        }

        long long select_query::count()
        {
            // sucks to backup the columns just for this
            // query, but I don't see an easy way right now
            // TODO: improve
            auto cols(columns_);

            try {
                columns_ = {"COUNT(*)"};

                prepare(to_sql());

                sql_number value = execute_scalar<sql_number>();

                columns_ = cols;

                return value.as<long long>();
            } catch (...) {
                // make sure we don't leave this query in a temp state
                columns_ = cols;

                return -1;
            }
        }

        resultset select_query::execute()
        {
            prepare(to_sql());

            return stmt_->results();
        }

        void select_query::execute(const std::function<void(const resultset &rs)> &funk)
        {
            prepare(to_sql());

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
            out << other.to_sql();
            return out;
        }
    }
}

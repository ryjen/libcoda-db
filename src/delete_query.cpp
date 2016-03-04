#include "delete_query.h"
#include "log.h"

using namespace std;

namespace arg3
{
    namespace db
    {

        delete_query::delete_query(sqldb *db, const std::string &tableName) : modify_query(db)
        {
            tableName_ = tableName;
        }

        delete_query::delete_query(const shared_ptr<schema> &schema) : modify_query(schema)
        {
            tableName_ = schema->table_name();
        }

        delete_query::delete_query(const delete_query &other) : modify_query(other), where_(other.where_), tableName_(other.tableName_)
        {
        }
        delete_query::delete_query(delete_query &&other)
            : modify_query(std::move(other)), where_(std::move(other.where_)), tableName_(std::move(other.tableName_))
        {
        }

        delete_query::~delete_query()
        {
        }

        delete_query &delete_query::operator=(const delete_query &other)
        {
            modify_query::operator=(other);
            where_ = other.where_;
            tableName_ = other.tableName_;
            return *this;
        }

        delete_query &delete_query::operator=(delete_query &&other)
        {
            modify_query::operator=(std::move(other));
            where_ = std::move(other.where_);
            tableName_ = std::move(other.tableName_);
            return *this;
        }

        bool delete_query::is_valid() const
        {
            return query::is_valid() && !tableName_.empty();
        }

        delete_query &delete_query::where(const where_clause &value)
        {
            where_ = value;
            return *this;
        }

        where_clause &delete_query::where(const string &value)
        {
            where_ = where_clause(value);
            return where_;
        }

        delete_query &delete_query::from(const std::string &value)
        {
            tableName_ = value;
            return *this;
        }

        std::string delete_query::from() const
        {
            return tableName_;
        }

        string delete_query::to_string() const
        {
            ostringstream buf;

            buf << "DELETE FROM " << tableName_;

            if (!where_.empty()) {
                buf << " WHERE " << where_;
            } else {
                log::warn("empty where clause for delete query");
            }

            buf << ";";

            return buf.str();
        }
    }
}

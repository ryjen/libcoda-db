#include "delete_query.h"
#include "exception.h"
#include "sqldb.h"
#include "log.h"

namespace arg3
{
    namespace db
    {
        delete_query::delete_query(sqldb *db, const string &tableName) : query(db), tableName_(tableName)
        {
            if (tableName.empty()) {
                throw database_exception("No table name provided for query");
            }
        }
        delete_query::delete_query(shared_ptr<schema> schema) : delete_query(schema->db(), schema->table_name())
        {
        }
        delete_query::delete_query(const delete_query &other) : query(other), where_(other.where_), tableName_(other.tableName_)
        {
        }

        delete_query::delete_query(delete_query &&other)
            : query(std::move(other)), where_(std::move(other.where_)), tableName_(std::move(other.tableName_))
        {
        }

        delete_query::~delete_query()
        {
        }
        delete_query &delete_query::operator=(const delete_query &other)
        {
            query::operator=(other);
            where_ = other.where_;
            tableName_ = other.tableName_;

            return *this;
        }

        delete_query &delete_query::operator=(delete_query &&other)
        {
            query::operator=(std::move(other));
            where_ = std::move(other.where_);
            tableName_ = std::move(other.tableName_);

            return *this;
        }

        delete_query &delete_query::where(const where_clause &value)
        {
            where_ = value;

            return *this;
        }

        delete_query &delete_query::where(const string &value)
        {
            where_ = where_clause(value);

            return *this;
        }

        string delete_query::table_name() const
        {
            return tableName_;
        }
        delete_query &delete_query::table_name(const string &value)
        {
            if (value.empty()) {
                throw database_exception("No table name provided for query");
            }
            tableName_ = value;
            return *this;
        }

        string delete_query::to_string() const
        {
            ostringstream buf;

            buf << "DELETE FROM " << tableName_;

            if (!where_.empty()) {
                buf << " WHERE " << where_.to_string();
            }

            return buf.str();
        }

        int delete_query::execute(bool batch)
        {
            prepare(to_string());

            int res = stmt_->result() ? stmt_->last_number_of_changes() : 0;

            if (!batch) {
                stmt_->finish();
                stmt_ = nullptr;
            } else {
                reset();
            }

            return res;
        }
    }
}

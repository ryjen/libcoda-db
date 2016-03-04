#include "update_query.h"
#include "log.h"

using namespace std;

namespace arg3
{
    namespace db
    {

        /*!
         * @param db the database to modify
         * @param tableName the table to modify
         * @param columns the columns to modify
         */
        update_query::update_query(sqldb *db, const std::string &tableName) : modify_query(db)
        {
            tableName_ = tableName;
        }

        /*!
         * @param db the database to modify
         * @param columns the columns to modify
         */
        update_query::update_query(sqldb *db, const std::string &tableName, const std::vector<std::string> &columns) : modify_query(db)
        {
            tableName_ = tableName;
            columns_ = columns;
        }

        /*!
         * @param schema the schema to modify
         * @param column the specific columns to modify in the schema
         */
        update_query::update_query(const std::shared_ptr<schema> &schema, const std::vector<std::string> &columns) : modify_query(schema)
        {
            tableName_ = schema->table_name();
            columns_ = columns;
        }

        update_query::update_query(const update_query &other)
            : modify_query(other), where_(other.where_), columns_(other.columns_), tableName_(other.tableName_)
        {
        }
        update_query::update_query(update_query &&other)
            : modify_query(std::move(other)),
              where_(std::move(other.where_)),
              columns_(std::move(other.columns_)),
              tableName_(std::move(other.tableName_))
        {
        }

        update_query::~update_query()
        {
        }
        update_query &update_query::operator=(const update_query &other)
        {
            modify_query::operator=(other);
            where_ = other.where_;
            columns_ = other.columns_;
            tableName_ = other.tableName_;
            return *this;
        }

        update_query &update_query::operator=(update_query &&other)
        {
            modify_query::operator=(std::move(other));
            where_ = std::move(other.where_);
            columns_ = std::move(other.columns_);
            tableName_ = std::move(other.tableName_);
            return *this;
        }

        bool update_query::is_valid() const
        {
            return query::is_valid() && !tableName_.empty();
        }

        string update_query::table() const
        {
            return tableName_;
        }

        update_query &update_query::table(const string &value)
        {
            tableName_ = value;
            return *this;
        }

        string update_query::to_string() const
        {
            ostringstream buf;

            buf << "UPDATE " << tableName_;

            if (columns_.size() > 0) {
                buf << " SET ";
                buf << join_params(columns_, true);
            }

            if (!where_.empty()) {
                buf << " WHERE " << where_;
            } else {
                log::warn("empty where clause for update query");
            }

            buf << ";";

            return buf.str();
        }

        update_query &update_query::columns(const vector<string> &columns)
        {
            columns_ = columns;
            return *this;
        }

        vector<string> update_query::columns() const
        {
            return columns_;
        }

        update_query &update_query::where(const where_clause &value)
        {
            where_ = value;
            return *this;
        }

        where_clause &update_query::where(const string &value)
        {
            where_ = where_clause(value);
            return where_;
        }

    }
}

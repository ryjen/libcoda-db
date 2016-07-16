#include "insert_query.h"
#include "log.h"
#include "schema.h"
#include "session.h"
#include "statement.h"

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
        insert_query::insert_query(const std::shared_ptr<arg3::db::session> &session, const std::string &tableName) : modify_query(session)
        {
            tableName_ = tableName;
        }

        /*!
         * @param db the database to modify
         * @param columns the columns to modify
         */
        insert_query::insert_query(const std::shared_ptr<arg3::db::session> &session, const std::string &tableName,
                                   const std::vector<std::string> &columns)
            : modify_query(session)
        {
            tableName_ = tableName;
            columns_ = columns;
        }

        /*!
         * @param schema the schema to modify
         * @param column the specific columns to modify in the schema
         */
        insert_query::insert_query(const std::shared_ptr<schema> &schema, const std::vector<std::string> &columns)
            : modify_query(schema->get_session())
        {
            tableName_ = schema->table_name();
            columns_ = columns;
        }


        insert_query::insert_query(const insert_query &other)
            : modify_query(other), lastId_(other.lastId_), columns_(other.columns_), tableName_(other.tableName_)
        {
        }
        insert_query::insert_query(insert_query &&other)
            : modify_query(std::move(other)), lastId_(other.lastId_), columns_(std::move(other.columns_)), tableName_(std::move(other.tableName_))
        {
        }

        insert_query::~insert_query()
        {
        }
        insert_query &insert_query::operator=(const insert_query &other)
        {
            modify_query::operator=(other);
            lastId_ = other.lastId_;
            columns_ = other.columns_;
            tableName_ = other.tableName_;
            return *this;
        }

        insert_query &insert_query::operator=(insert_query &&other)
        {
            modify_query::operator=(std::move(other));
            lastId_ = other.lastId_;
            columns_ = std::move(other.columns_);
            tableName_ = std::move(other.tableName_);
            return *this;
        }

        bool insert_query::is_valid() const
        {
            return query::is_valid() && !tableName_.empty();
        }

        long long insert_query::last_insert_id() const
        {
            return lastId_;
        }

        string insert_query::to_string() const
        {
            if (session_ == nullptr) {
                throw database_exception("invalid query, no database");
            }
            auto schema = session_->get_schema(tableName_);

            if (schema == nullptr) {
                throw database_exception("invalid query, schema not found or initialized yet");
            }

            return session_->insert_sql(session_->get_schema(tableName_), columns_);
        }

        insert_query &insert_query::columns(const vector<string> &columns)
        {
            columns_ = columns;
            return *this;
        }

        vector<string> insert_query::columns() const
        {
            return columns_;
        }

        int insert_query::execute()
        {
            if (!is_valid()) {
                throw database_exception("invalid insert query");
            }

            prepare(to_string());

            bool success = stmt_->result();

            if (success) {
                numChanges_ = stmt_->last_number_of_changes();
                lastId_ = stmt_->last_insert_id();
            } else {
                lastId_ = 0;
                numChanges_ = 0;
            }

            // if (flags_ & Batch) {
            stmt_->reset();
            //} else {
            //    stmt_->finish();
            //    stmt_ = nullptr;
            //}

            return numChanges_;
        }

        insert_query &insert_query::into(const std::string &value)
        {
            tableName_ = value;
            return *this;
        }

        std::string insert_query::into() const
        {
            return tableName_;
        }

        insert_query &insert_query::values(const std::vector<sql_value> &value)
        {
            bindable::bind(value);
            return *this;
        }

        insert_query &insert_query::values(const std::unordered_map<std::string, sql_value> &value)
        {
            bindable::bind(value);
            return *this;
        }
    }
}

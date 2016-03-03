/*!
 * @copyright ryan jennings (arg3.com), 2013
 */
#include "modify_query.h"
#include "exception.h"
#include "log.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        modify_query::modify_query(sqldb *db) : query(db), flags_(0), numChanges_(0)
        {
        }

        modify_query::modify_query(const shared_ptr<schema> &schema) : modify_query(schema->db())
        {
        }

        modify_query::modify_query(const modify_query &other) : query(other), flags_(other.flags_), numChanges_(other.numChanges_)
        {
        }
        modify_query::modify_query(modify_query &&other) : query(std::move(other)), flags_(other.flags_), numChanges_(other.numChanges_)
        {
        }

        modify_query::~modify_query()
        {
        }
        modify_query &modify_query::operator=(const modify_query &other)
        {
            query::operator=(other);
            flags_ = other.flags_;
            numChanges_ = other.numChanges_;
            return *this;
        }

        modify_query &modify_query::operator=(modify_query &&other)
        {
            query::operator=(std::move(other));
            flags_ = other.flags_;
            numChanges_ = other.numChanges_;
            return *this;
        }

        bool insert_query::is_valid() const
        {
            return query::is_valid() && !tableName_.empty();
        }

        bool update_query::is_valid() const
        {
            return query::is_valid() && !tableName_.empty();
        }

        bool delete_query::is_valid() const
        {
            return query::is_valid() && !tableName_.empty();
        }

        /*!
         * @param db the database to modify
         * @param tableName the table to modify
         * @param columns the columns to modify
         */
        insert_query::insert_query(sqldb *db, const std::string &tableName) : modify_query(db)
        {
            tableName_ = tableName;
        }

        /*!
         * @param db the database to modify
         * @param columns the columns to modify
         */
        insert_query::insert_query(sqldb *db, const std::string &tableName, const std::vector<std::string> &columns) : modify_query(db)
        {
            tableName_ = tableName;
            columns_ = columns;
        }

        /*!
         * @param schema the schema to modify
         * @param column the specific columns to modify in the schema
         */
        insert_query::insert_query(const std::shared_ptr<schema> &schema, const std::vector<std::string> &columns) : modify_query(schema->db())
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

        string update_query::table() const
        {
            return tableName_;
        }

        update_query &update_query::table(const string &value)
        {
            tableName_ = value;
            return *this;
        }

        long long insert_query::last_insert_id() const
        {
            return lastId_;
        }

        int modify_query::last_number_of_changes() const
        {
            return numChanges_;
        }

        modify_query &modify_query::set_flags(int value)
        {
            flags_ = value;
            return *this;
        }

        string insert_query::to_string() const
        {
            if (db_ == nullptr) {
                throw database_exception("invalid query, no database");
            }
            auto schema = db_->schemas()->get(tableName_);

            if (schema == nullptr) {
                throw database_exception("invalid query, schema not found or initialized yet");
            }

            return db_->insert_sql(db_->schemas()->get(tableName_), columns_);
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

        insert_query &insert_query::columns(const vector<string> &columns)
        {
            columns_ = columns;
            return *this;
        }

        vector<string> insert_query::columns() const
        {
            return columns_;
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
                log::error(stmt_->last_error().c_str());
                lastId_ = 0;
                numChanges_ = 0;
            }

            if (flags_ & Batch) {
                reset();
            } else {
                stmt_->finish();
                stmt_ = nullptr;
            }

            return numChanges_;
        }

        int modify_query::execute()
        {
            if (!is_valid()) {
                throw database_exception("Invalid modify query");
            }

            prepare(to_string());

            bool success = stmt_->result();

            if (success) {
                numChanges_ = stmt_->last_number_of_changes();
            } else {
                log::error(stmt_->last_error().c_str());
                numChanges_ = 0;
            }

            if (flags_ & Batch) {
                reset();
            } else {
                stmt_->finish();
                stmt_ = nullptr;
            }

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

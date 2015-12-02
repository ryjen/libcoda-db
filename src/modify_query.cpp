/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "modify_query.h"
#include "exception.h"
#include "log.h"

namespace arg3
{
    namespace db
    {
        modify_query::modify_query(sqldb *db, const string &tableName, const vector<string> &columns)
            : query(db), columns_(columns), tableName_(tableName), flags_(0)
        {
            if (tableName.empty()) {
                throw database_exception("No table name provided for modify query");
            }
        }

        modify_query::modify_query(sqldb *db, const string &tableName) : query(db), tableName_(tableName), flags_(0)
        {
            if (tableName.empty()) {
                throw database_exception("No table name provided for modify query");
            }
        }
        modify_query::modify_query(shared_ptr<schema> schema) : modify_query(schema->db(), schema->table_name(), schema->column_names())
        {
        }

        modify_query::modify_query(const modify_query &other)
            : query(other), columns_(other.columns_), tableName_(other.tableName_), flags_(other.flags_)
        {
        }
        modify_query::modify_query(modify_query &&other)
            : query(std::move(other)), columns_(std::move(other.columns_)), tableName_(std::move(other.tableName_)), flags_(other.flags_)
        {
        }

        modify_query::~modify_query()
        {
        }
        modify_query &modify_query::operator=(const modify_query &other)
        {
            query::operator=(other);
            columns_ = other.columns_;
            tableName_ = other.tableName_;
            flags_ = other.flags_;
            return *this;
        }

        modify_query &modify_query::operator=(modify_query &&other)
        {
            query::operator=(std::move(other));
            columns_ = std::move(other.columns_);
            tableName_ = std::move(other.tableName_);
            flags_ = other.flags_;
            return *this;
        }

        string modify_query::table_name() const
        {
            return tableName_;
        }
        modify_query &modify_query::table_name(const string &value)
        {
            tableName_ = value;
            return *this;
        }

        long long insert_query::last_insert_id() const
        {
            return lastId_;
        }

        modify_query &modify_query::set_flags(int value)
        {
            flags_ = value;

            return *this;
        }

        string insert_query::to_string() const
        {
            ostringstream buf;

            buf << "INSERT INTO " << tableName_;

            if (columns_.size() > 0) {
                buf << "(";

                buf << join_csv(columns_);

                buf << ") VALUES (";

                buf << join_csv('?', columns_.size());

                buf << ")";
            } else {
                log::warn("No binded values for modify query");
                buf << " DEFAULT VALUES";
            }

            return buf.str();
        }

        string update_query::to_string() const
        {
            ostringstream buf;

            buf << "UPDATE " << tableName_;

            if (columns_.size() > 0) {
                buf << " SET ";

                for (int i = 0, size = columns_.size(); i < size; i++) {
                    buf << columns_[i] << " = ?";
                    if (i + 1 < size) {
                        buf << ", ";
                    }
                }
            } else {
                throw database_exception("No binding values provided for update query");
            }

            if (!where_.empty()) {
                buf << " WHERE " << where_;
            } else {
                log::warn("empty where clause for update");
            }

            return buf.str();
        }

        update_query &update_query::where(const where_clause &value)
        {
            where_ = value;
            return *this;
        }

        update_query &update_query::where(const string &value)
        {
            where_ = where_clause(value);
            return *this;
        }

        string modify_query::to_string() const
        {
            ostringstream buf;

            buf << "REPLACE INTO " << tableName_;

            if (columns_.size() > 0) {
                buf << "(";

                buf << join_csv(columns_);

                buf << ") VALUES (";

                buf << join_csv('?', columns_.size());

                buf << ")";
            } else {
                log::warn("No values provided for modify query");
                buf << " DEFAULT VALUES";
            }

            return buf.str();
        }

        int insert_query::execute()
        {
            prepare(to_string());

            bool success = stmt_->result();

            int res = 0;

            if (success) {
                res = stmt_->last_number_of_changes();
                lastId_ = stmt_->last_insert_id();
            } else {
                log::trace(stmt_->last_error().c_str());
            }

            if (flags_ & BATCH) {
                reset();
            } else {
                stmt_->finish();
                stmt_ = nullptr;
            }

            return res;
        }

        int modify_query::execute()
        {
            prepare(to_string());

            bool success = stmt_->result();

            int res = 0;

            if (success) {
                res = stmt_->last_number_of_changes();
            } else {
                log::trace(stmt_->last_error().c_str());
            }

            if (flags_ & BATCH) {
                reset();
            } else {
                stmt_->finish();
                stmt_ = nullptr;
            }

            return res;
        }

        string delete_query::to_string() const
        {
            ostringstream buf;

            buf << "DELETE FROM " << tableName_;

            if (!where_.empty()) {
                buf << " WHERE " << where_;
            }

            return buf.str();
        }
    }
}

/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "modify_query.h"
#include "exception.h"

namespace arg3
{
    namespace db
    {
        modify_query::modify_query(sqldb *db, const string &tableName, const vector<string> &columns)
            : query(db), columns_(columns), tableName_(tableName)
        {
        }

        modify_query::modify_query(sqldb *db, const string &tableName) : query(db), tableName_(tableName)
        {
        }
        modify_query::modify_query(shared_ptr<schema> schema) : modify_query(schema->db(), schema->table_name(), schema->column_names())
        {
        }

        modify_query::modify_query(const modify_query &other) : query(other), columns_(other.columns_), tableName_(other.tableName_)
        {
        }
        modify_query::modify_query(modify_query &&other)
            : query(std::move(other)), columns_(std::move(other.columns_)), tableName_(std::move(other.tableName_))
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
            return *this;
        }

        modify_query &modify_query::operator=(modify_query &&other)
        {
            query::operator=(std::move(other));
            columns_ = std::move(other.columns_);
            tableName_ = std::move(other.tableName_);

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
        string modify_query::to_insert_string() const
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
                buf << " DEAULT VALUES";
            }

            return buf.str();
        }

        string modify_query::to_update_string(const std::string &idColumnName) const
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
            }

            buf << " WHERE " << idColumnName << " = ?";

            return buf.str();
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
                buf << " DEFAULT VALUES";
            }

            return buf.str();
        }

        int modify_query::executeUpdate(const std::string &idColumnName, bool batch)
        {
            prepare(to_update_string(idColumnName));

            bool success = stmt_->result();

            int res = 0;

            if (success) {
                res = stmt_->last_number_of_changes();
            }
            if (!batch) {
                stmt_->finish();
                stmt_ = nullptr;
            } else {
                reset();
            }

            return res;
        }

        int modify_query::executeInsert(long long *insertId, bool batch)
        {
            prepare(to_insert_string());

            bool success = stmt_->result();

            int res = 0;

            if (success) {
                res = stmt_->last_number_of_changes();
                if (insertId) *insertId = stmt_->last_insert_id();
            }
            if (!batch) {
                stmt_->finish();
                stmt_ = nullptr;
            } else {
                reset();
            }

            return res;
        }

        int modify_query::execute(long long *insertId, bool batch)
        {
            prepare(to_string());

            bool success = stmt_->result();

            int res = 0;

            if (success) {
                res = stmt_->last_number_of_changes();
                if (insertId) *insertId = stmt_->last_insert_id();
            }
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

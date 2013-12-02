/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "modify_query.h"
#include "exception.h"

namespace arg3
{
    namespace db
    {
        modify_query::modify_query(sqldb *db, const string &tableName,
                                   const vector<string> &columns) : base_query(db, tableName), columns_(columns)
        {}

        modify_query::modify_query(sqldb *db, const string &tableName) : base_query(db, tableName)
        {}

        modify_query::modify_query(shared_ptr<schema> schema) : modify_query(schema->db(), schema->table_name(), schema->column_names())
        {}

        modify_query::modify_query(const modify_query &other) : base_query(other), columns_(other.columns_)
        {}

        modify_query::modify_query(modify_query &&other) : base_query(std::move(other)), columns_(std::move(other.columns_))
        {}

        modify_query::~modify_query() {}

        modify_query &modify_query::operator=(const modify_query &other)
        {
            base_query::operator=(other);
            columns_ = other.columns_;
            return *this;
        }

        modify_query &modify_query::operator=(modify_query && other)
        {
            base_query::operator=(std::move(other));
            columns_ = std::move(other.columns_);

            return *this;
        }

        string modify_query::to_string() const
        {
            ostringstream buf;

            buf << "REPLACE INTO " << tableName_;

            if (columns_.size() > 0)
            {
                buf << "(";

                buf << join_csv(columns_);

                buf << ") VALUES (";

                buf << join_csv('?', columns_.size());

                buf << ")";
            }
            else
            {
                buf << " DEFAULT VALUES";
            }

            return buf.str();
        }

        bool modify_query::execute(bool batch)
        {
            prepare();

            int res = sqlite3_step(stmt_);

            if (!batch)
            {
                if (sqlite3_finalize(stmt_) != SQLITE_OK)
                    throw database_exception(db_->last_error());

                stmt_ = NULL;
            }
            else
            {
                if (sqlite3_reset(stmt_) != SQLITE_OK)
                    throw database_exception(db_->last_error());
            }

            return res == SQLITE_DONE;
        }
    }
}
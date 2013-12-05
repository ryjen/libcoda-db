#include "sqlite3_resultset.h"
#include "sqlite3_db.h"
#include "sqlite3_row.h"

namespace arg3
{
    namespace db
    {
        sqlite3_resultset::sqlite3_resultset(sqlite3_db *db, sqlite3_stmt *stmt) : stmt_(stmt), db_(db), status_(-1)
        {

        }

        sqlite3_resultset::sqlite3_resultset(const sqlite3_resultset &other) : stmt_(other.stmt_), db_(other.db_), status_(other.status_)
        {}

        sqlite3_resultset::sqlite3_resultset(sqlite3_resultset &&other) : stmt_(other.stmt_), db_(other.db_), status_(other.status_)
        {
            other.db_ = NULL;
            other.stmt_ = NULL;
        }

        sqlite3_resultset::~sqlite3_resultset() {}

        sqlite3_resultset &sqlite3_resultset::operator=(const sqlite3_resultset &other)
        {
            if (this != &other)
            {
                stmt_ = other.stmt_;
                db_ = other.db_;
                status_ = other.status_;
            }

            return *this;
        }

        sqlite3_resultset &sqlite3_resultset::operator=(sqlite3_resultset && other)
        {
            if (this != &other)
            {
                stmt_ = other.stmt_;
                db_ = other.db_;
                status_ = other.status_;
                other.db_ = NULL;
                other.stmt_ = NULL;
            }

            return *this;
        }

        bool sqlite3_resultset::is_valid() const
        {
            return stmt_ != NULL;
        }

        bool sqlite3_resultset::next()
        {
            if (status_ == SQLITE_DONE)
                return false;

            status_ = sqlite3_step(stmt_);

            return status_ == SQLITE_ROW;
        }
        void sqlite3_resultset::reset()
        {
            if (sqlite3_reset(stmt_) != SQLITE_OK)
                throw database_exception(db_->last_error());
        }

        row sqlite3_resultset::current_row()
        {
            return row(make_shared<sqlite3_row>(db_, stmt_));
        }

    }
}
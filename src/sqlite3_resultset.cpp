#include "sqlite3_resultset.h"
#include "sqlite3_db.h"
#include "sqlite3_row.h"

#ifdef HAVE_LIBSQLITE3

namespace arg3
{
    namespace db
    {
        sqlite3_resultset::sqlite3_resultset(sqlite3_db *db, shared_ptr<sqlite3_stmt> stmt) : stmt_(stmt), db_(db), status_(-1)
        {

        }

        sqlite3_resultset::sqlite3_resultset(sqlite3_resultset &&other) : stmt_(other.stmt_), db_(other.db_), status_(other.status_)
        {
            other.db_ = NULL;
            other.stmt_ = nullptr;
        }

        sqlite3_resultset::~sqlite3_resultset() {}

        sqlite3_resultset &sqlite3_resultset::operator=(sqlite3_resultset && other)
        {
            stmt_ = other.stmt_;
            db_ = other.db_;
            status_ = other.status_;
            other.db_ = NULL;
            other.stmt_ = nullptr;

            return *this;
        }

        bool sqlite3_resultset::is_valid() const
        {
            return stmt_ != nullptr && stmt_;
        }

        size_t sqlite3_resultset::size() const
        {
            return sqlite3_column_count(stmt_.get());
        }

        bool sqlite3_resultset::next()
        {
            if (!is_valid())
                return false;

            if (status_ == SQLITE_DONE)
                return false;

            status_ = sqlite3_step(stmt_.get());

            return status_ == SQLITE_ROW;
        }

        void sqlite3_resultset::reset()
        {
            if (sqlite3_reset(stmt_.get()) != SQLITE_OK)
                throw database_exception(db_->last_error());
        }

        row sqlite3_resultset::current_row()
        {
            if (db_->cache_level() == sqldb::CACHE_ROW)
                return row(make_shared<sqlite3_cached_row>(db_, stmt_));
            else
                return row(make_shared<sqlite3_row>(db_, stmt_));
        }

        /* cached version */

        sqlite3_cached_resultset::sqlite3_cached_resultset(sqlite3_db *db, shared_ptr<sqlite3_stmt> stmt) : db_(db), currentRow_(-1)
        {
            int status = sqlite3_step(stmt.get());

            while (status == SQLITE_ROW)
            {
                rows_.push_back(make_shared<sqlite3_cached_row>(db, stmt));

                status = sqlite3_step(stmt.get());
            }
        }

        sqlite3_cached_resultset::sqlite3_cached_resultset(sqlite3_cached_resultset &&other) : db_(other.db_), rows_(other.rows_), currentRow_(other.currentRow_)
        {
            other.db_ = NULL;
        }

        sqlite3_cached_resultset::~sqlite3_cached_resultset() {}

        sqlite3_cached_resultset &sqlite3_cached_resultset::operator=(sqlite3_cached_resultset && other)
        {
            db_ = other.db_;
            rows_ = other.rows_;
            currentRow_ = other.currentRow_;
            other.db_ = NULL;

            return *this;
        }

        bool sqlite3_cached_resultset::is_valid() const
        {
            return db_ != NULL;
        }

        size_t sqlite3_cached_resultset::size() const
        {
            return rows_.size();
        }

        bool sqlite3_cached_resultset::next()
        {
            if (rows_.empty()) return false;

            return ++currentRow_ < static_cast<int>(rows_.size());
        }
        void sqlite3_cached_resultset::reset()
        {
            currentRow_ = -1;
        }

        row sqlite3_cached_resultset::current_row()
        {
            if (currentRow_ >= 0 && currentRow_ < static_cast<int>(rows_.size()))
                return row(rows_[currentRow_]);
            else
                return row();
        }

    }
}

#endif


#include "resultset.h"
#include "db.h"
#include "row.h"
#include "../log.h"

#ifdef HAVE_LIBSQLITE3

using namespace std;

namespace arg3
{
    namespace db
    {
        namespace sqlite
        {
            resultset::resultset(sqlite::db *db, const shared_ptr<sqlite3_stmt> &stmt) : stmt_(stmt), db_(db), status_(-1)
            {
                if (db_ == NULL) {
                    throw database_exception("No database provided to sqlite3 resultset");
                }

                if (stmt_ == nullptr) {
                    throw database_exception("no statement provided to sqlite3 resultset");
                }
            }

            resultset::resultset(resultset &&other) : stmt_(other.stmt_), db_(other.db_), status_(other.status_)
            {
                other.db_ = NULL;
                other.stmt_ = nullptr;
            }

            resultset::~resultset()
            {
            }

            resultset &resultset::operator=(resultset &&other)
            {
                stmt_ = other.stmt_;
                db_ = other.db_;
                status_ = other.status_;
                other.db_ = NULL;
                other.stmt_ = nullptr;

                return *this;
            }

            bool resultset::is_valid() const
            {
                return stmt_ != nullptr && stmt_;
            }

            size_t resultset::size() const
            {
                if (!is_valid()) {
                    return 0;
                }
                return sqlite3_column_count(stmt_.get());
            }

            bool resultset::next()
            {
                if (!is_valid()) {
                    return false;
                }

                if (status_ == SQLITE_DONE) {
                    return false;
                }

                status_ = sqlite3_step(stmt_.get());

                return status_ == SQLITE_ROW;
            }

            void resultset::reset()
            {
                if (!is_valid()) {
                    log::warn("resultset::reset database not open");
                    return;
                }

                if (sqlite3_reset(stmt_.get()) != SQLITE_OK) {
                    throw database_exception(db_->last_error());
                }
            }

            resultset::row_type resultset::current_row()
            {
                if (db_->cache_level() == cache::Row)
                    return row_type(make_shared<cached_row>(db_, stmt_));
                else
                    return row_type(make_shared<row>(db_, stmt_));
            }

            /* cached version */

            cached_resultset::cached_resultset(sqlite::db *db, shared_ptr<sqlite3_stmt> stmt) : db_(db), currentRow_(-1)
            {
                if (stmt == nullptr) {
                    throw database_exception("cached_resultset:: invalidate statement");
                }

                int status = sqlite3_step(stmt.get());

                while (status == SQLITE_ROW) {
                    rows_.push_back(make_shared<cached_row>(db, stmt));

                    status = sqlite3_step(stmt.get());
                }
            }

            cached_resultset::cached_resultset(cached_resultset &&other) : db_(other.db_), rows_(other.rows_), currentRow_(other.currentRow_)
            {
                other.db_ = NULL;
            }

            cached_resultset::~cached_resultset()
            {
            }

            cached_resultset &cached_resultset::operator=(cached_resultset &&other)
            {
                db_ = other.db_;
                rows_ = other.rows_;
                currentRow_ = other.currentRow_;
                other.db_ = NULL;

                return *this;
            }

            bool cached_resultset::is_valid() const
            {
                return db_ != NULL;
            }

            size_t cached_resultset::size() const
            {
                return rows_.size();
            }

            bool cached_resultset::next()
            {
                if (rows_.empty()) return false;

                return static_cast<unsigned>(++currentRow_) < rows_.size();
            }

            void cached_resultset::reset()
            {
                currentRow_ = -1;
            }

            cached_resultset::row_type cached_resultset::current_row()
            {
                if (currentRow_ >= 0 && static_cast<unsigned>(currentRow_) < rows_.size())
                    return row_type(rows_[currentRow_]);
                else
                    return row_type();
            }
        }
    }
}

#endif

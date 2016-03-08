#include "resultset.h"
#include "session.h"
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
            resultset::resultset(const std::shared_ptr<sqlite::session> &sess, const shared_ptr<sqlite3_stmt> &stmt)
                : stmt_(stmt), sess_(sess), status_(-1)
            {
                if (sess_ == NULL) {
                    throw database_exception("No database provided to sqlite3 resultset");
                }

                if (stmt_ == nullptr) {
                    throw database_exception("no statement provided to sqlite3 resultset");
                }
            }

            resultset::resultset(resultset &&other) : stmt_(std::move(other.stmt_)), sess_(std::move(other.sess_)), status_(other.status_)
            {
                other.sess_ = NULL;
                other.stmt_ = nullptr;
            }

            resultset::~resultset()
            {
            }

            resultset &resultset::operator=(resultset &&other)
            {
                stmt_ = std::move(other.stmt_);
                sess_ = std::move(other.sess_);
                status_ = other.status_;
                other.sess_ = NULL;
                other.stmt_ = nullptr;

                return *this;
            }

            bool resultset::is_valid() const
            {
                return stmt_ != nullptr && stmt_;
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
                    throw database_exception(sess_->last_error());
                }
                status_ = -1;
            }

            resultset::row_type resultset::current_row()
            {
                if (sess_->cache_level() == cache::Row)
                    return row_type(make_shared<cached_row>(sess_, stmt_));
                else
                    return row_type(make_shared<row>(sess_, stmt_));
            }

            /* cached version */

            cached_resultset::cached_resultset(const std::shared_ptr<sqlite::session> &sess, shared_ptr<sqlite3_stmt> stmt)
                : sess_(sess), currentRow_(-1)
            {
                if (stmt == nullptr) {
                    throw database_exception("postgres cached resultset invalidate statement");
                }

                int status = sqlite3_step(stmt.get());

                while (status == SQLITE_ROW) {
                    rows_.push_back(make_shared<cached_row>(sess, stmt));

                    status = sqlite3_step(stmt.get());
                }
            }

            cached_resultset::cached_resultset(cached_resultset &&other)
                : sess_(std::move(other.sess_)), rows_(std::move(other.rows_)), currentRow_(other.currentRow_)
            {
                other.sess_ = NULL;
            }

            cached_resultset::~cached_resultset()
            {
            }

            cached_resultset &cached_resultset::operator=(cached_resultset &&other)
            {
                sess_ = std::move(other.sess_);
                rows_ = std::move(other.rows_);
                currentRow_ = other.currentRow_;
                other.sess_ = NULL;

                return *this;
            }

            bool cached_resultset::is_valid() const
            {
                return sess_ != NULL;
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

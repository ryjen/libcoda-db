#include "postgres_resultset.h"
#include "postgres_db.h"
#include "postgres_row.h"

#ifdef HAVE_LIBPQ

namespace arg3
{
    namespace db
    {
        postgres_resultset::postgres_resultset(postgres_db *db, const shared_ptr<PGresult> &stmt) : stmt_(stmt), db_(db), currentRow_(-1)
        {
            if (db_ == nullptr) {
                throw database_exception("No database provided to postgres resultset");
            }

            if (stmt_ == nullptr) {
                throw database_exception("no statement provided to postgres resultset");
            }
        }

        postgres_resultset::postgres_resultset(postgres_resultset &&other)
            : stmt_(std::move(other.stmt_)), db_(other.db_), currentRow_(other.currentRow_)
        {
            other.db_ = nullptr;
            other.stmt_ = nullptr;
        }

        postgres_resultset::~postgres_resultset()
        {
            stmt_ = nullptr;
        }

        postgres_resultset &postgres_resultset::operator=(postgres_resultset &&other)
        {
            stmt_ = std::move(other.stmt_);
            db_ = other.db_;
            currentRow_ = other.currentRow_;
            other.db_ = nullptr;
            other.stmt_ = nullptr;

            return *this;
        }

        bool postgres_resultset::is_valid() const
        {
            return stmt_ != nullptr && stmt_;
        }

        size_t postgres_resultset::size() const
        {
            return PQntuples(stmt_.get());
        }

        bool postgres_resultset::next()
        {
            if (!is_valid()) {
                return false;
            }

            return ++currentRow_ < static_cast<int>(size());
        }

        void postgres_resultset::reset()
        {
            currentRow_ = -1;
        }

        row postgres_resultset::current_row()
        {
            if (db_->cache_level() == sqldb::CACHE_ROW)
                return row(make_shared<postgres_cached_row>(db_, stmt_, currentRow_));
            else
                return row(make_shared<postgres_row>(db_, stmt_, currentRow_));
        }

        /* cached version */

        postgres_cached_resultset::postgres_cached_resultset(postgres_db *db, const shared_ptr<PGresult> &stmt) : db_(db), currentRow_(-1)
        {
            for (size_t i = 0; i < PQntuples(stmt.get()); i++) {
                rows_.push_back(make_shared<postgres_cached_row>(db, stmt, i));
            }
        }

        postgres_cached_resultset::postgres_cached_resultset(postgres_cached_resultset &&other)
            : db_(other.db_), rows_(std::move(other.rows_)), currentRow_(other.currentRow_)
        {
            other.db_ = nullptr;
        }

        postgres_cached_resultset::~postgres_cached_resultset()
        {
        }

        postgres_cached_resultset &postgres_cached_resultset::operator=(postgres_cached_resultset &&other)
        {
            db_ = other.db_;
            rows_ = std::move(other.rows_);
            currentRow_ = other.currentRow_;
            other.db_ = nullptr;

            return *this;
        }

        bool postgres_cached_resultset::is_valid() const
        {
            return db_ != nullptr;
        }

        size_t postgres_cached_resultset::size() const
        {
            return rows_.size();
        }

        bool postgres_cached_resultset::next()
        {
            if (rows_.empty()) return false;

            return ++currentRow_ < static_cast<int>(rows_.size());
        }
        void postgres_cached_resultset::reset()
        {
            currentRow_ = -1;
        }

        row postgres_cached_resultset::current_row()
        {
            if (currentRow_ >= 0 && currentRow_ < static_cast<int>(rows_.size()))
                return row(rows_[currentRow_]);
            else
                return row();
        }
    }
}

#endif

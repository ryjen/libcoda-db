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
            int value = PQntuples(stmt_.get());

            if (value < 0) {
                return 0;
            }

            return value;
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
            return row(make_shared<postgres_row>(db_, stmt_, currentRow_));
        }
    }
}

#endif

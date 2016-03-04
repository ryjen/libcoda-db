#include "resultset.h"
#include "db.h"
#include "row.h"

#ifdef HAVE_LIBPQ

using namespace std;

namespace arg3
{
    namespace db
    {
        namespace postgres
        {
            resultset::resultset(postgres::db *db, const shared_ptr<PGresult> &stmt) : stmt_(stmt), db_(db), currentRow_(-1)
            {
                if (db_ == nullptr) {
                    throw database_exception("No database provided to postgres resultset");
                }

                if (stmt_ == nullptr) {
                    throw database_exception("no statement provided to postgres resultset");
                }
            }

            resultset::resultset(resultset &&other) : stmt_(std::move(other.stmt_)), db_(other.db_), currentRow_(other.currentRow_)
            {
                other.db_ = nullptr;
                other.stmt_ = nullptr;
            }

            resultset::~resultset()
            {
            }

            resultset &resultset::operator=(resultset &&other)
            {
                stmt_ = std::move(other.stmt_);
                db_ = other.db_;
                currentRow_ = other.currentRow_;
                other.db_ = nullptr;
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

                int value = PQntuples(stmt_.get());

                if (value < 0) {
                    return 0;
                }

                return value;
            }

            bool resultset::next()
            {
                if (!is_valid()) {
                    return false;
                }

                return static_cast<unsigned>(++currentRow_) < size();
            }

            void resultset::reset()
            {
                currentRow_ = -1;
            }

            resultset::row_type resultset::current_row()
            {
                return row_type(make_shared<row>(db_, stmt_, currentRow_));
            }
        }
    }
}

#endif

#include "row.h"
#include "column.h"
#include "db.h"

#ifdef HAVE_LIBPQ

namespace arg3
{
    namespace db
    {
        postgres_row::postgres_row(postgres_db *db, const shared_ptr<PGresult> &stmt, int row) : row_impl(), stmt_(stmt), db_(db), row_(row)
        {
            if (db_ == NULL) {
                throw database_exception("no database provided to postgres row");
            }

            if (stmt_ == nullptr) {
                throw database_exception("no statement provided to postgres row");
            }

            size_ = PQnfields(stmt_.get());
        }

        postgres_row::postgres_row(postgres_row &&other)
            : row_impl(std::move(other)), stmt_(std::move(other.stmt_)), db_(other.db_), size_(other.size_), row_(other.row_)
        {
            other.stmt_ = nullptr;
            other.db_ = NULL;
        }

        postgres_row::~postgres_row()
        {
            stmt_ = nullptr;
        }


        postgres_row &postgres_row::operator=(postgres_row &&other)
        {
            stmt_ = std::move(other.stmt_);
            db_ = other.db_;
            size_ = other.size_;
            row_ = other.row_;
            other.stmt_ = nullptr;
            other.db_ = NULL;

            return *this;
        }

        row_impl::column_type postgres_row::column(size_t nPosition) const
        {
            if (nPosition >= size() || row_ == -1) {
                throw no_such_column_exception();
            }

            return row_impl::column_type(make_shared<postgres_column>(stmt_, row_, nPosition));
        }

        row_impl::column_type postgres_row::column(const string &name) const
        {
            if (name.empty()) {
                throw no_such_column_exception();
            }

            for (size_t i = 0; i < size_; i++) {
                const char *col_name = PQfname(stmt_.get(), i);

                if (name == col_name) {
                    return column(i);
                }
            }
            throw no_such_column_exception(name);
        }

        string postgres_row::column_name(size_t nPosition) const
        {
            if (nPosition >= size()) {
                throw no_such_column_exception();
            }

            return PQfname(stmt_.get(), nPosition);
        }

        size_t postgres_row::size() const
        {
            return size_;
        }

        bool postgres_row::is_valid() const
        {
            return stmt_ != nullptr && row_ >= 0;
        }
    }
}

#endif

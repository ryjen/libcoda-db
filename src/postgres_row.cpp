#include "postgres_row.h"
#include "postgres_column.h"
#include "postgres_db.h"

#ifdef HAVE_LIBPQ

namespace arg3
{
    namespace db
    {
        postgres_row::postgres_row(postgres_db *db, shared_ptr<PGresult> stmt, size_t row) : row_impl(), stmt_(stmt), db_(db), row_(row)
        {
            if (db_ == NULL) {
                throw database_exception("no database provided to postgres row");
            }

            if (stmt_ == nullptr) {
                throw database_exception("no statement provided to postgres row");
            }

            size_ = PQnfields(stmt_.get());

            if (row_ >= PQntuples(stmt_.get())) {
                throw database_exception("invalid row number provided to postgres row");
            }
        }

        postgres_row::postgres_row(postgres_row &&other)
            : row_impl(std::move(other)), stmt_(other.stmt_), db_(other.db_), size_(other.size_), row_(other.row_)
        {
            other.stmt_ = nullptr;
            other.db_ = NULL;
        }

        postgres_row::~postgres_row()
        {
        }


        postgres_row &postgres_row::operator=(postgres_row &&other)
        {
            stmt_ = other.stmt_;
            db_ = other.db_;
            size_ = other.size_;
            row_ = other.row_;
            other.stmt_ = nullptr;
            other.db_ = NULL;

            return *this;
        }

        row_impl::column_type postgres_row::column(size_t nPosition) const
        {
            if (nPosition >= size()) {
                throw no_such_column_exception();
            }

            if (db_->cache_level() == sqldb::CACHE_COLUMN)
                return row_impl::column_type(make_shared<postgres_cached_column>(stmt_, row_, nPosition));
            else
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
            return stmt_ != nullptr;
        }


        /* cached version */

        postgres_cached_row::postgres_cached_row(postgres_db *db, shared_ptr<PGresult> stmt, size_t row)
        {
            if (db == NULL) {
                throw database_exception("no database provided to postgres row");
            }

            if (stmt == nullptr) {
                throw database_exception("no statement provided to postgres row");
            }

            size_t size = PQnfields(stmt.get());

            for (int i = 0; i < size; i++) {
                columns_.push_back(make_shared<postgres_cached_column>(stmt, row, i));
            }
        }
        postgres_cached_row::postgres_cached_row(postgres_cached_row &&other) : columns_(std::move(other.columns_))
        {
            other.columns_.clear();
        }

        postgres_cached_row::~postgres_cached_row()
        {
        }

        postgres_cached_row &postgres_cached_row::operator=(postgres_cached_row &&other)
        {
            columns_ = other.columns_;
            other.columns_.clear();

            return *this;
        }

        row_impl::column_type postgres_cached_row::column(size_t nPosition) const
        {
            if (nPosition >= size()) {
                throw no_such_column_exception();
            }

            return row_impl::column_type(columns_[nPosition]);
        }

        row_impl::column_type postgres_cached_row::column(const string &name) const
        {
            if (name.empty()) {
                throw no_such_column_exception();
            }

            for (size_t i = 0; i < columns_.size(); i++) {
                if (name == column_name(i)) {
                    return column(i);
                }
            }
            throw no_such_column_exception(name);
        }

        string postgres_cached_row::column_name(size_t nPosition) const
        {
            if (nPosition >= size()) {
                throw no_such_column_exception();
            }

            return columns_[nPosition]->name();
        }

        size_t postgres_cached_row::size() const
        {
            return columns_.size();
        }

        bool postgres_cached_row::is_valid() const
        {
            return columns_.size() > 0;
        }
    }
}

#endif

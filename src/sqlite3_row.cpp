#include "sqlite3_row.h"
#include "sqlite3_column.h"

namespace arg3
{
    namespace db
    {

        sqlite3_row::sqlite3_row(sqlite3_db *db, sqlite3_stmt *stmt) : row_impl(), stmt_(stmt), db_(db)
        {
            assert(db_ != NULL);

            assert(stmt_ != NULL);

            size_ = sqlite3_column_count(stmt_);
        }

        sqlite3_row::sqlite3_row(sqlite3_row &&other) : row_impl(std::move(other)), stmt_(other.stmt_), db_(other.db_), size_(other.size_)
        {
            other.stmt_ = NULL;
            other.db_ = NULL;
        }

        sqlite3_row::~sqlite3_row() {}


        sqlite3_row &sqlite3_row::operator=(sqlite3_row && other)
        {
            stmt_ = other.stmt_;
            db_ = other.db_;
            size_ = other.size_;
            other.stmt_ = NULL;
            other.db_ = NULL;

            return *this;
        }

        column sqlite3_row::column(size_t nPosition) const
        {
            assert(nPosition < size());

            return db::column(make_shared<sqlite3_column>( stmt_, nPosition ) );
        }

        column sqlite3_row::column(const string &name) const
        {
            assert(!name.empty());

            for (size_t i = 0; i < size_; i++)
            {
                const char *col_name = sqlite3_column_name(stmt_, i);

                if (name == col_name)
                {
                    return column(i);
                }
            }
            throw database_exception("unknown column '" + name + "'");
        }

        string sqlite3_row::column_name(size_t nPosition) const
        {
            assert(nPosition < size());

            return sqlite3_column_name(stmt_, nPosition);
        }

        size_t sqlite3_row::size() const
        {
            return size_;
        }

        bool sqlite3_row::is_valid() const
        {
            return stmt_ !=  NULL;
        }

    }
}
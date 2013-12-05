/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "row.h"
#include "resultset.h"

namespace arg3
{
    namespace db
    {
        row::row(shared_ptr<row_impl> impl) : impl_(impl)
        {}

        row::row(const row &other) : impl_(other.impl_)
        {}

        row::row(row &&other) : impl_(std::move(other.impl_))
        {
            other.impl_ = nullptr;
        }

        row::~row()
        {}

        row &row::operator=(const row &other)
        {
            if (this != &other)
            {
                impl_ = other.impl_;
            }
            return *this;
        }
        row &row::operator=(row && other)
        {
            if (this != &other)
            {
                impl_ = std::move(other.impl_);
                other.impl_ = nullptr;
            }
            return *this;
        }

        sqlite3_row::sqlite3_row(sqlite3_db *db, sqlite3_stmt *stmt) : row_impl(), stmt_(stmt), db_(db)
        {
            assert(db_ != NULL);

            assert(stmt_ != NULL);

            size_ = sqlite3_column_count(stmt_);
        }

        sqlite3_row::sqlite3_row(const sqlite3_row &other) : row_impl(other), stmt_(other.stmt_), db_(other.db_), size_(other.size_)
        {}

        sqlite3_row::sqlite3_row(sqlite3_row &&other) : row_impl(other), stmt_(other.stmt_), db_(other.db_), size_(other.size_)
        {
            other.stmt_ = NULL;
            other.db_ = NULL;
        }

        sqlite3_row::~sqlite3_row() {}

        sqlite3_row &sqlite3_row::operator=(const sqlite3_row &other)
        {
            if (this != &other)
            {
                stmt_ = other.stmt_;
                db_ = other.db_;
                size_ = other.size_;
            }
            return *this;
        }

        sqlite3_row &sqlite3_row::operator=(sqlite3_row && other)
        {
            if (this != &other)
            {
                stmt_ = other.stmt_;
                db_ = other.db_;
                size_ = other.size_;
                other.stmt_ = NULL;
                other.db_ = NULL;
            }
            return *this;
        }

        row::iterator row::begin()
        {
            return iterator(this, 0);
        }

        row::const_iterator row::begin() const
        {
            return const_iterator(this, 0);
        }

        row::const_iterator row::cbegin() const
        {
            return begin();
        }

        row::iterator row::end()
        {
            return iterator(this, size());
        }

        row::const_iterator row::end() const
        {
            return const_iterator(this, size());
        }

        row::const_iterator row::cend() const
        {
            return end();
        }

        row::reverse_iterator row::rbegin()
        {
            return reverse_iterator(end());
        }

        row::const_reverse_iterator row::rbegin() const
        {
            return const_reverse_iterator(end());
        }

        row::const_reverse_iterator row::crbegin() const
        {
            return rbegin();
        }

        row::reverse_iterator row::rend()
        {
            return reverse_iterator(begin());
        }

        row::const_reverse_iterator row::rend() const
        {
            return const_reverse_iterator(begin());
        }

        row::const_reverse_iterator row::crend() const
        {
            return rend();
        }

        column row::operator[](size_t nPosition) const
        {
            return column(nPosition);
        }

        column row::operator[](const string &name) const
        {
            return column(name);
        }


        string row::column_name(size_t nPosition) const
        {
            return impl_->column_name(nPosition);
        }

        arg3::db::column row::column(size_t nPosition) const
        {
            return impl_->column(nPosition);
        }

        arg3::db::column row::column(const string &name) const
        {
            return impl_->column(name);
        }

        size_t row::size() const
        {
            return impl_->size();
        }

        column sqlite3_row::column(size_t nPosition) const
        {
            assert(nPosition < size());

            return db::column( sqlite3_column_value(stmt_, nPosition ) );
        }

        column sqlite3_row::column(const string &name) const
        {
            assert(!name.empty());

            for (size_t i = 0, size = sqlite3_column_count(stmt_); i < size; i++)
            {
                const char *col_name = sqlite3_column_name(stmt_, i);

                if (name == col_name)
                {
                    return column(i);
                }
            }
            return arg3::db::column();
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

        bool row::empty() const
        {
            return size() == 0;
        }
    }
}
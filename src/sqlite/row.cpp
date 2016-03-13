#include "row.h"
#include "column.h"
#include "session.h"

#ifdef HAVE_LIBSQLITE3

using namespace std;

namespace arg3
{
    namespace db
    {
        namespace sqlite
        {
            row::row(const std::shared_ptr<sqlite::session> &sess, const shared_ptr<sqlite3_stmt> &stmt) : row_impl(), stmt_(stmt), sess_(sess)
            {
                if (sess_ == NULL) {
                    throw database_exception("no database provided to sqlite3 row");
                }

                if (stmt_ == nullptr) {
                    throw database_exception("no statement provided to sqlite3 row");
                }

                size_ = sqlite3_column_count(stmt_.get());
            }

            row::row(row &&other) : row_impl(std::move(other)), stmt_(std::move(other.stmt_)), sess_(std::move(other.sess_)), size_(other.size_)
            {
                other.stmt_ = nullptr;
                other.sess_ = nullptr;
            }

            row::~row()
            {
            }


            row &row::operator=(row &&other)
            {
                stmt_ = std::move(other.stmt_);
                sess_ = std::move(other.sess_);
                size_ = other.size_;
                other.stmt_ = nullptr;
                other.sess_ = nullptr;

                return *this;
            }

            row::column_type row::column(size_t nPosition) const
            {
                if (nPosition >= size()) {
                    throw no_such_column_exception();
                }

                if (sess_->cache_level() == cache::Column)
                    return column_type(make_shared<cached_column>(stmt_, nPosition));
                else
                    return column_type(make_shared<sqlite::column>(stmt_, nPosition));
            }

            row::column_type row::column(const string &name) const
            {
                if (name.empty()) {
                    throw no_such_column_exception();
                }

                for (size_t i = 0; i < size_; i++) {
                    const char *col_name = sqlite3_column_name(stmt_.get(), i);

                    if (name == col_name) {
                        return column(i);
                    }
                }
                throw no_such_column_exception(name);
            }

            string row::column_name(size_t nPosition) const
            {
                if (nPosition >= size()) {
                    throw no_such_column_exception();
                }

                return sqlite3_column_name(stmt_.get(), nPosition);
            }

            size_t row::size() const
            {
                return size_;
            }

            bool row::is_valid() const
            {
                return stmt_ != nullptr;
            }


            /* cached version */

            cached_row::cached_row(const std::shared_ptr<sqlite::session> &sess, shared_ptr<sqlite3_stmt> stmt)
            {
                if (sess == NULL) {
                    throw database_exception("no database provided to sqlite3 row");
                }

                if (stmt == nullptr) {
                    throw database_exception("no statement provided to sqlite3 row");
                }

                int size = sqlite3_column_count(stmt.get());

                for (int i = 0; i < size; i++) {
                    columns_.push_back(make_shared<cached_column>(stmt, i));
                }
            }
            cached_row::cached_row(cached_row &&other) : columns_(std::move(other.columns_))
            {
                other.columns_.clear();
            }

            cached_row::~cached_row()
            {
            }

            cached_row &cached_row::operator=(cached_row &&other)
            {
                columns_ = std::move(other.columns_);
                other.columns_.clear();

                return *this;
            }

            cached_row::column_type cached_row::column(size_t nPosition) const
            {
                if (nPosition >= size()) {
                    throw no_such_column_exception();
                }

                return column_type(columns_[nPosition]);
            }

            cached_row::column_type cached_row::column(const string &name) const
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

            string cached_row::column_name(size_t nPosition) const
            {
                if (nPosition >= size()) {
                    throw no_such_column_exception();
                }

                return columns_[nPosition]->name();
            }

            size_t cached_row::size() const
            {
                return columns_.size();
            }

            bool cached_row::is_valid() const
            {
                return columns_.size() > 0;
            }
        }
    }
}

#endif

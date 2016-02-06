#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBMYSQLCLIENT

#include "mysql_row.h"
#include "mysql_column.h"
#include "mysql_binding.h"
#include "mysql_db.h"

namespace arg3
{
    namespace db
    {
        mysql_row::mysql_row(mysql_db *db, const shared_ptr<MYSQL_RES> &res, MYSQL_ROW row) : row_impl(), row_(row), res_(res), db_(db)
        {
            if (db_ == nullptr) {
                throw database_exception("no database provided for mysql row");
            }

            if (row_ == nullptr) {
                throw database_exception("no raw data for mysql row");
            }

            if (res_ == nullptr) {
                throw database_exception("no mysql result provided for row");
            }

            size_ = mysql_num_fields(res.get());
        }

        mysql_row::mysql_row(mysql_row &&other) : row_impl(std::move(other)), row_(other.row_), res_(other.res_), db_(other.db_), size_(other.size_)
        {
            other.row_ = nullptr;
            other.db_ = nullptr;
            other.res_ = nullptr;
        }

        mysql_row::~mysql_row()
        {
        }

        mysql_row &mysql_row::operator=(mysql_row &&other)
        {
            row_ = other.row_;
            res_ = other.res_;
            db_ = other.db_;
            size_ = other.size_;
            other.row_ = nullptr;
            other.db_ = nullptr;
            other.res_ = nullptr;

            return *this;
        }

        row_impl::column_type mysql_row::column(size_t position) const
        {
            if (!is_valid()) {
                throw database_exception("invalid row");
            }

            if (position >= size()) {
                throw no_such_column_exception();
            }

            // if (db_->cache_level() == cache::Column)
            //     return row_impl::column_type(make_shared<mysql_cached_column>(res_, row_, position));
            // else
            return row_impl::column_type(make_shared<mysql_column>(res_, row_, position));
        }

        row_impl::column_type mysql_row::column(const string &name) const
        {
            if (!is_valid()) {
                throw database_exception("invalid mysql row");
            }

            if (name.empty()) {
                throw no_such_column_exception();
            }

            for (size_t i = 0; i < size_; i++) {
                auto field = mysql_fetch_field_direct(res_.get(), i);

                if (field != nullptr && field->name != nullptr && name == field->name) {
                    return column(i);
                }
            }
            throw no_such_column_exception(name);
        }

        string mysql_row::column_name(size_t position) const
        {
            if (!is_valid()) {
                throw database_exception("invalid mysql row");
            }

            if (position >= size()) {
                throw no_such_column_exception();
            }

            auto field = mysql_fetch_field_direct(res_.get(), position);

            if (field == nullptr || field->name == nullptr) return string();

            return field->name;
        }

        size_t mysql_row::size() const
        {
            return size_;
        }

        bool mysql_row::is_valid() const
        {
            return res_ != nullptr && res_ && row_ != nullptr;
        }


        /* statement version */


        mysql_stmt_row::mysql_stmt_row(mysql_db *db, const shared_ptr<MYSQL_STMT> &stmt, const shared_ptr<MYSQL_RES> &metadata,
                                       const shared_ptr<mysql_binding> &fields)
            : row_impl(), fields_(fields), metadata_(metadata), stmt_(stmt), db_(db), size_(0)
        {
            if (db_ == nullptr) {
                throw database_exception("No database provided for mysql row");
            }

            if (metadata_ != nullptr) {
                size_ = mysql_num_fields(metadata_.get());
            }
        }

        mysql_stmt_row::mysql_stmt_row(mysql_stmt_row &&other)
            : row_impl(std::move(other)),
              fields_(std::move(other.fields_)),
              metadata_(std::move(other.metadata_)),
              stmt_(std::move(other.stmt_)),
              db_(other.db_),
              size_(other.size_)
        {
            other.fields_ = nullptr;
            other.db_ = nullptr;
            other.metadata_ = nullptr;
            other.stmt_ = nullptr;
        }

        mysql_stmt_row::~mysql_stmt_row()
        {
        }

        mysql_stmt_row &mysql_stmt_row::operator=(mysql_stmt_row &&other)
        {
            fields_ = std::move(other.fields_);
            metadata_ = std::move(other.metadata_);
            db_ = other.db_;
            size_ = other.size_;
            stmt_ = std::move(other.stmt_);
            other.fields_ = nullptr;
            other.db_ = nullptr;
            other.metadata_ = nullptr;
            other.stmt_ = nullptr;

            return *this;
        }

        row_impl::column_type mysql_stmt_row::column(size_t position) const
        {
            if (!is_valid()) {
                throw database_exception("invalid mysql row");
            }

            if (position >= size()) {
                throw no_such_column_exception();
            }

            // if (db_->cache_level() == cache::Column)
            //     return row_impl::column_type(make_shared<mysql_cached_column>(column_name(position), *fields_.get(), position));
            // else
            return row_impl::column_type(make_shared<mysql_stmt_column>(column_name(position), fields_, position));
        }

        row_impl::column_type mysql_stmt_row::column(const string &name) const
        {
            if (!is_valid()) {
                throw database_exception("invalid mysql row");
            }

            if (name.empty()) {
                throw no_such_column_exception();
            }

            for (size_t i = 0; i < size(); i++) {
                auto field = mysql_fetch_field_direct(metadata_.get(), i);

                if (field != nullptr && field->name != nullptr && name == field->name) {
                    return column(i);
                }
            }
            throw no_such_column_exception(name);
        }

        string mysql_stmt_row::column_name(size_t position) const
        {
            if (!is_valid()) {
                throw database_exception("invalid mysql row");
            }
            if (position >= size()) {
                throw no_such_column_exception();
            }

            auto field = mysql_fetch_field_direct(metadata_.get(), position);

            if (field == nullptr || field->name == nullptr) return string();

            return field->name;
        }

        size_t mysql_stmt_row::size() const
        {
            return size_;
        }

        bool mysql_stmt_row::is_valid() const
        {
            return fields_ != nullptr && metadata_ != nullptr;
        }
    }
}

#endif

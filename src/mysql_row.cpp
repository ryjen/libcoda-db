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
        mysql_row::mysql_row(mysql_db *db, shared_ptr<MYSQL_RES> res, MYSQL_ROW row) : row_impl(), row_(row), res_(res), db_(db)
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

        mysql_row::mysql_row(const mysql_row &other) : row_impl(other), row_(other.row_), res_(other.res_), db_(other.db_), size_(other.size_)
        {
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

        mysql_row &mysql_row::operator=(const mysql_row &other)
        {
            row_ = other.row_;
            res_ = other.res_;
            db_ = other.db_;
            size_ = other.size_;

            return *this;
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

        row_impl::column_type mysql_row::column(size_t nPosition) const
        {
            if (!is_valid()) {
                throw database_exception("invalid row");
            }

            if (nPosition >= size()) {
                throw no_such_column_exception();
            }

            if (db_->cache_level() == sqldb::CACHE_COLUMN)
                return row_impl::column_type(make_shared<mysql_cached_column>(res_, row_, nPosition));
            else
                return row_impl::column_type(make_shared<mysql_column>(res_, row_, nPosition));
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

        string mysql_row::column_name(size_t nPosition) const
        {
            if (!is_valid()) {
                throw database_exception("invalid mysql row");
            }

            if (nPosition >= size()) {
                throw no_such_column_exception();
            }

            auto field = mysql_fetch_field_direct(res_.get(), nPosition);

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


        mysql_stmt_row::mysql_stmt_row(mysql_db *db, shared_ptr<MYSQL_STMT> stmt, shared_ptr<MYSQL_RES> metadata, shared_ptr<mysql_binding> fields)
            : row_impl(), fields_(fields), metadata_(metadata), stmt_(stmt), db_(db), size_(0)
        {
            if (db_ == nullptr) {
                throw database_exception("No database provided for mysql row");
            }

            if (metadata_ != nullptr) {
                size_ = mysql_num_fields(metadata_.get());
            }
        }

        mysql_stmt_row::mysql_stmt_row(const mysql_stmt_row &other)
            : row_impl(other), fields_(other.fields_), metadata_(other.metadata_), stmt_(other.stmt_), db_(other.db_), size_(other.size_)
        {
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

        mysql_stmt_row &mysql_stmt_row::operator=(const mysql_stmt_row &other)
        {
            fields_ = other.fields_;
            metadata_ = other.metadata_;
            db_ = other.db_;
            size_ = other.size_;
            stmt_ = other.stmt_;

            return *this;
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

        row_impl::column_type mysql_stmt_row::column(size_t nPosition) const
        {
            if (!is_valid()) {
                throw database_exception("invalid mysql row");
            }

            if (nPosition >= size()) {
                throw no_such_column_exception();
            }

            if (db_->cache_level() == sqldb::CACHE_COLUMN)
                return row_impl::column_type(make_shared<mysql_cached_column>(column_name(nPosition), *fields_.get(), nPosition));
            else
                return row_impl::column_type(make_shared<mysql_stmt_column>(column_name(nPosition), fields_, nPosition));
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

        string mysql_stmt_row::column_name(size_t nPosition) const
        {
            if (!is_valid()) {
                throw database_exception("invalid mysql row");
            }
            if (nPosition >= size()) {
                throw no_such_column_exception();
            }

            auto field = mysql_fetch_field_direct(metadata_.get(), nPosition);

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

        /* cached version */

        mysql_cached_row::mysql_cached_row(sqldb *db, shared_ptr<MYSQL_RES> metadata, mysql_binding &fields)
        {
            if (metadata == nullptr) {
                throw database_exception("No meta data provided for mysql cached row");
            }

            size_t size = mysql_num_fields(metadata.get());

            for (size_t i = 0; i < size; i++) {
                auto field = mysql_fetch_field_direct(metadata.get(), i);

                if (field == nullptr || field->name == nullptr) continue;

                columns_.push_back(make_shared<mysql_cached_column>(field->name, fields, i));
            }
        }

        mysql_cached_row::mysql_cached_row(sqldb *db, shared_ptr<MYSQL_RES> res, MYSQL_ROW row)
        {
            if (row == nullptr || res == nullptr) {
                throw database_exception("missing data for mysql cached row");
            }

            size_t size = mysql_num_fields(res.get());

            for (size_t i = 0; i < size; i++) {
                columns_.push_back(make_shared<mysql_cached_column>(res, row, i));
            }
        }

        row_impl::column_type mysql_cached_row::column(size_t nPosition) const
        {
            if (nPosition >= columns_.size()) {
                throw no_such_column_exception();
            }

            return row_impl::column_type(columns_[nPosition]);
        }

        row_impl::column_type mysql_cached_row::column(const string &name) const
        {
            for (size_t i = 0; i < columns_.size(); i++) {
                if (name == column_name(i)) {
                    return column(i);
                }
            }
            throw no_such_column_exception(name);
        }

        string mysql_cached_row::column_name(size_t nPosition) const
        {
            if (nPosition >= columns_.size()) {
                throw database_exception("invalid index for row column");
            }
            return columns_[nPosition]->name();
        }

        size_t mysql_cached_row::size() const
        {
            return columns_.size();
        }

        bool mysql_cached_row::is_valid() const
        {
            return true;
        }
    }
}

#endif

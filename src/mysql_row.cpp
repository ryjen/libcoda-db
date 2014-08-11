#include "config.h"


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
            assert(db_ != NULL);

            assert(row_ != NULL);

            assert(res_ != nullptr);

            size_ = mysql_num_fields(res.get());
        }

        mysql_row::mysql_row(const mysql_row &other) : row_impl(other), row_(other.row_), res_(other.res_), db_(other.db_), size_(other.size_)
        {}

        mysql_row::mysql_row(mysql_row &&other) : row_impl(std::move(other)), row_(other.row_), res_(other.res_), db_(other.db_), size_(other.size_)
        {
            other.row_ = NULL;
            other.db_ = NULL;
            other.res_ = nullptr;
        }

        mysql_row::~mysql_row() {}

        mysql_row &mysql_row::operator=(const mysql_row &other)
        {
            row_ = other.row_;
            res_ = other.res_;
            db_ = other.db_;
            size_ = other.size_;

            return *this;
        }

        mysql_row &mysql_row::operator=(mysql_row && other)
        {
            row_ = other.row_;
            res_ = other.res_;
            db_ = other.db_;
            size_ = other.size_;
            other.row_ = NULL;
            other.db_ = NULL;
            other.res_ = nullptr;

            return *this;
        }

        column mysql_row::column(size_t nPosition) const
        {
            if (!is_valid())
                throw database_exception("invalid row");

            if (nPosition >= size())
            {
                throw database_exception("invalid index for row column");
            }

            if (db_->cache_level() == sqldb::CACHE_COLUMN)
                return db::column(make_shared<mysql_cached_column>( res_, row_, nPosition ));
            else
                return db::column( make_shared<mysql_column>( res_, row_, nPosition ) );
        }

        column mysql_row::column(const string &name) const
        {
            assert(!name.empty());

            assert(res_ != nullptr);

            for (size_t i = 0; i < size_; i++)
            {
                auto field = mysql_fetch_field_direct(res_.get(), i);

                if (name == field->name)
                {
                    return column(i);
                }
            }
            throw database_exception("unknown column '" + name + "'");
        }

        string mysql_row::column_name(size_t nPosition) const
        {
            assert(res_ != nullptr);

            if (nPosition >= size())
            {
                throw database_exception("invalid index for row column");
            }

            auto field = mysql_fetch_field_direct(res_.get(), nPosition);

            return field->name;
        }

        size_t mysql_row::size() const
        {
            return size_;
        }

        bool mysql_row::is_valid() const
        {
            return res_ != nullptr && res_ && row_ != NULL;
        }


        /* statement version */


        mysql_stmt_row::mysql_stmt_row(mysql_db *db, shared_ptr<MYSQL_RES> metadata, shared_ptr<mysql_binding> fields) : row_impl(), fields_(fields), metadata_(metadata),
            db_(db)
        {
            assert(db_ != NULL);

            //assert(metadata_ != NULL);

            //assert(fields_ != nullptr);

            if (metadata_ != nullptr)
                size_ = mysql_num_fields(metadata_.get());
        }

        mysql_stmt_row::mysql_stmt_row(const mysql_stmt_row &other) : row_impl(other), fields_(other.fields_), metadata_(other.metadata_), db_(other.db_)
        {}

        mysql_stmt_row::mysql_stmt_row(mysql_stmt_row &&other) : row_impl(std::move(other)), fields_(other.fields_), metadata_(other.metadata_), db_(other.db_)
        {
            other.fields_ = nullptr;
            other.db_ = NULL;
            other.metadata_ = nullptr;
        }

        mysql_stmt_row::~mysql_stmt_row() {}

        mysql_stmt_row &mysql_stmt_row::operator=(const mysql_stmt_row &other)
        {
            fields_ = other.fields_;
            metadata_ = other.metadata_;
            db_ = other.db_;
            size_ = other.size_;

            return *this;
        }

        mysql_stmt_row &mysql_stmt_row::operator=(mysql_stmt_row && other)
        {
            fields_ = other.fields_;
            metadata_ = other.metadata_;
            db_ = other.db_;
            size_ = other.size_;
            other.fields_ = nullptr;
            other.db_ = NULL;
            other.metadata_ = nullptr;

            return *this;
        }

        column mysql_stmt_row::column(size_t nPosition) const
        {
            assert(fields_ != nullptr);

            if (!is_valid())
            {
                throw database_exception("invalid row");
            }

            if (nPosition >= size())
            {
                throw database_exception("invalid index for row column");
            }

            if (db_->cache_level() == sqldb::CACHE_COLUMN)
                return db::column(make_shared<mysql_cached_column>( column_name(nPosition), *fields_.get(), nPosition));
            else
                return db::column(make_shared<mysql_stmt_column>( column_name(nPosition), fields_, nPosition ) );
        }

        column mysql_stmt_row::column(const string &name) const
        {
            assert(!name.empty());

            assert(metadata_ != nullptr);

            if (!is_valid())
            {
                throw database_exception("invalid row");
            }

            for (size_t i = 0; i < size(); i++)
            {
                auto field = mysql_fetch_field_direct(metadata_.get(), i);

                if (name == field->name)
                {
                    return column(i);
                }
            }
            throw database_exception("unknown column '" + name + "'");
        }

        string mysql_stmt_row::column_name(size_t nPosition) const
        {
            assert(metadata_ != nullptr);

            if (nPosition >= size())
            {
                throw database_exception("invalid index for row column");
            }

            auto field = mysql_fetch_field_direct(metadata_.get(), nPosition);

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
            //assert(fields != nullptr);

            if (metadata != nullptr)
            {
                int size = mysql_num_fields(metadata.get());

                for (size_t i = 0; i < size; i++)
                {
                    auto field = mysql_fetch_field_direct(metadata.get(), i);

                    columns_.push_back(make_shared<mysql_cached_column>(field->name, fields, i));
                }
            }
        }

        mysql_cached_row::mysql_cached_row(sqldb *db, shared_ptr<MYSQL_RES> res, MYSQL_ROW row)
        {
            assert(row != NULL);

            assert(res != NULL);

            int size = mysql_num_fields(res.get());

            for (size_t i = 0; i < size; i++)
            {
                columns_.push_back(make_shared<mysql_cached_column>(res, row, i));
            }
        }

        column mysql_cached_row::column(size_t nPosition) const
        {
            assert(nPosition < size());

            return arg3::db::column(columns_[nPosition]);
        }

        column mysql_cached_row::column(const string &name) const
        {
            assert(!name.empty());

            for (int i = 0; i < columns_.size(); i++)
            {
                if (name == column_name(i))
                    return column(i);
            }
            throw database_exception("unknown column '" + name + "'");
        }

        string mysql_cached_row::column_name(size_t nPosition) const
        {
            assert(nPosition < size());

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

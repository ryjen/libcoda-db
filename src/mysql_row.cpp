#include "config.h"


#ifdef HAVE_LIBMYSQLCLIENT

#include "mysql_row.h"
#include "mysql_column.h"

namespace arg3
{
    namespace db
    {

        mysql_row::mysql_row(mysql_db *db, MYSQL_RES *res, MYSQL_ROW row) : row_impl(), row_(row), res_(res), db_(db)
        {
            assert(db_ != NULL);

            assert(row_ != NULL);

            assert(res_ != NULL);

            size_ = mysql_num_fields(res);
        }

        mysql_row::mysql_row(const mysql_row &other) : row_impl(other), row_(other.row_), res_(other.res_), db_(other.db_), size_(other.size_)
        {}

        mysql_row::mysql_row(mysql_row &&other) : row_impl(std::move(other)), row_(other.row_), res_(other.res_), db_(other.db_), size_(other.size_)
        {
            other.row_ = NULL;
            other.db_ = NULL;
            other.res_ = NULL;
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
            other.res_ = NULL;

            return *this;
        }

        column mysql_row::column(size_t nPosition) const
        {
            assert(nPosition < size());

            assert(is_valid());

            return db::column( make_shared<mysql_column>( res_, row_, nPosition ) );
        }

        column mysql_row::column(const string &name) const
        {
            assert(!name.empty());

            assert(res_ != NULL);

            for (size_t i = 0; i < size_; i++)
            {
                auto field = mysql_fetch_field_direct(res_, i);

                if (name == field->name)
                {
                    return column(i);
                }
            }
            throw database_exception("unknown column '" + name + "'");
        }

        string mysql_row::column_name(size_t nPosition) const
        {
            assert(nPosition < size());

            assert(res_ != NULL);

            auto field = mysql_fetch_field_direct(res_, nPosition);

            return field->name;
        }

        size_t mysql_row::size() const
        {
            return size_;
        }

        bool mysql_row::is_valid() const
        {
            return res_ != NULL && row_ != NULL;
        }


        /* statement version */

        mysql_stmt_row::mysql_stmt_row(mysql_db *db, MYSQL_RES *metadata, MYSQL_BIND *fields) : row_impl(), fields_(fields), metadata_(metadata),
            db_(db)
        {
            assert(db_ != NULL);

            assert(metadata_ != NULL);

            assert(fields_ != NULL);

            if (metadata_ != NULL)
                size_ = mysql_num_fields(metadata_);
        }

        mysql_stmt_row::mysql_stmt_row(const mysql_stmt_row &other) : row_impl(other), fields_(other.fields_), metadata_(other.metadata_), db_(other.db_)
        {}

        mysql_stmt_row::mysql_stmt_row(mysql_stmt_row &&other) : row_impl(std::move(other)), fields_(other.fields_), metadata_(other.metadata_), db_(other.db_)
        {
            other.fields_ = NULL;
            other.db_ = NULL;
            other.metadata_ = NULL;
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
            other.fields_ = NULL;
            other.db_ = NULL;
            other.metadata_ = NULL;

            return *this;
        }

        column mysql_stmt_row::column(size_t nPosition) const
        {
            assert(nPosition < size());

            assert(fields_ != NULL);

            return db::column(make_shared<mysql_stmt_column>( &fields_[nPosition] ) );
        }

        column mysql_stmt_row::column(const string &name) const
        {
            assert(!name.empty());

            assert(metadata_ != NULL);

            for (size_t i = 0; i < size_; i++)
            {
                auto field = mysql_fetch_field_direct(metadata_, i);

                if (name == field->name)
                {
                    return column(i);
                }
            }
            throw database_exception("unknown column '" + name + "'");
        }

        string mysql_stmt_row::column_name(size_t nPosition) const
        {
            assert(nPosition < size());

            assert(metadata_ != NULL);

            auto field = mysql_fetch_field_direct(metadata_, nPosition);

            return field->name;
        }

        size_t mysql_stmt_row::size() const
        {
            return size_;
        }

        bool mysql_stmt_row::is_valid() const
        {
            return fields_ != NULL && metadata_ != NULL;
        }
    }
}

#endif

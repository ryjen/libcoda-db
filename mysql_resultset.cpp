#include "mysql_resultset.h"
#include "mysql_db.h"
#include "mysql_row.h"
#include "exception.h"

namespace arg3
{
    namespace db
    {

        mysql_resultset::mysql_resultset(mysql_db *db, MYSQL_RES *res) : res_(res), row_(NULL), db_(db), refcount_(new unsigned(0))
        {

        }

        mysql_resultset::mysql_resultset(const mysql_resultset &other) : res_(other.res_), row_(other.row_), db_(other.db_), refcount_(other.refcount_)
        {
            (*refcount_)++;
        }

        mysql_resultset::mysql_resultset(mysql_resultset &&other) : res_(other.res_), row_(other.row_), db_(other.db_), refcount_(other.refcount_)
        {
            other.db_ = NULL;
            other.res_ = NULL;
            other.row_ = NULL;
            other.refcount_ = NULL;
        }

        mysql_resultset::~mysql_resultset()
        {
            if (refcount_)
            {
                if (*refcount_ > 0)
                {
                    (*refcount_)--;

                    if (*refcount_ != 0)
                        return;
                }

                delete refcount_;
                refcount_ = NULL;
            }

            if (res_ != NULL)
            {
                mysql_free_result(res_);
                res_ = NULL;
            }
        }

        mysql_resultset &mysql_resultset::operator=(const mysql_resultset &other)
        {
            res_ = other.res_;
            db_ = other.db_;
            row_ = other.row_;
            refcount_ = other.refcount_;
            (*refcount_)++;
            return *this;
        }

        mysql_resultset &mysql_resultset::operator=(mysql_resultset && other)
        {
            res_ = other.res_;
            db_ = other.db_;
            row_ = other.row_;
            refcount_ = other.refcount_;
            other.db_ = NULL;
            other.res_ = NULL;
            other.row_ = NULL;
            other.refcount_ = NULL;

            return *this;
        }

        bool mysql_resultset::is_valid() const
        {
            return res_ != NULL;
        }

        bool mysql_resultset::next()
        {
            if (res_ == NULL || !db_->is_open())
                return false;

            bool value = (row_ = mysql_fetch_row(res_)) != NULL;

            if (!value && !mysql_more_results(db_->db_))
            {
                mysql_free_result(res_);
                if ((res_ = mysql_use_result(db_->db_)) != NULL)
                    value = (row_ = mysql_fetch_row(res_)) != NULL;
            }

            return value;
        }

        void mysql_resultset::reset()
        {
            if (res_ != NULL)
                mysql_data_seek(res_, 0);
        }

        row mysql_resultset::current_row()
        {
            return row(make_shared<mysql_row>(db_, res_, row_));
        }

        size_t mysql_resultset::column_count() const
        {
            return mysql_num_fields(res_);
        }


        /* Statement version */

        extern string last_stmt_error(MYSQL_STMT *stmt);

        mysql_stmt_resultset::mysql_stmt_resultset(mysql_db *db, MYSQL_STMT *stmt) : stmt_(stmt), metadata_(NULL), db_(db),
            bindings_(NULL), columnCount_(0), refcount_(new unsigned(0)), status_(-1)
        {
        }

        mysql_stmt_resultset::mysql_stmt_resultset(const mysql_stmt_resultset &other) : stmt_(other.stmt_), metadata_(other.metadata_),
            db_(other.db_),
            bindings_(other.bindings_), columnCount_(other.columnCount_), refcount_(other.refcount_), status_(other.status_)
        {
            (*refcount_)++;

        }

        mysql_stmt_resultset::mysql_stmt_resultset(mysql_stmt_resultset &&other) : stmt_(other.stmt_), metadata_(other.metadata_),
            db_(other.db_),
            bindings_(other.bindings_), columnCount_(other.columnCount_), refcount_(other.refcount_), status_(other.status_)
        {
            other.db_ = NULL;
            other.stmt_ = NULL;
            other.bindings_ = NULL;
            other.metadata_ = NULL;
            other.refcount_ = NULL;
        }

        mysql_stmt_resultset::~mysql_stmt_resultset()
        {
            if (refcount_)
            {
                if (*refcount_ > 0)
                {
                    (*refcount_)--;

                    if (*refcount_ != 0)
                    {
                        return;
                    }
                }

                delete refcount_;
                refcount_ = NULL;
            }

            if (bindings_)
            {
                for (int i = 0; i < columnCount_; i++)
                {
                    if (bindings_[i].buffer)
                    {
                        free(bindings_[i].buffer);
                        bindings_[i].buffer = NULL;
                    }
                    if (bindings_[i].length)
                    {
                        free(bindings_[i].length);
                        bindings_[i].length = NULL;
                    }
                    if (bindings_[i].is_null)
                    {
                        free(bindings_[i].is_null);
                        bindings_[i].is_null = NULL;
                    }
                }
                free(bindings_);
                bindings_ = NULL;
            }

            if (metadata_)
            {
                mysql_free_result(metadata_);
                metadata_ = NULL;
            }
        }

        mysql_stmt_resultset &mysql_stmt_resultset::operator=(const mysql_stmt_resultset &other)
        {
            stmt_ = other.stmt_;
            db_ = other.db_;
            metadata_ = other.metadata_;
            bindings_ = other.bindings_;
            columnCount_ = other.columnCount_;
            refcount_ = other.refcount_;
            status_ = other.status_;

            (*refcount_)++;

            return *this;
        }

        mysql_stmt_resultset &mysql_stmt_resultset::operator=(mysql_stmt_resultset && other)
        {
            stmt_ = other.stmt_;
            db_ = other.db_;
            metadata_ = other.metadata_;
            bindings_ = other.bindings_;
            columnCount_ = other.columnCount_;
            refcount_ = other.refcount_;
            status_ = other.status_;
            other.db_ = NULL;
            other.bindings_ = NULL;
            other.metadata_ = NULL;
            other.refcount_ = NULL;

            return *this;
        }

        void mysql_stmt_resultset::get_results()
        {
            if (stmt_ == NULL || bindings_ != NULL || status_ != -1)
                return;

            if ((status_ = mysql_stmt_execute(stmt_)))
            {
                throw database_exception(last_stmt_error(stmt_));
            }

            metadata_ = mysql_stmt_result_metadata(stmt_);

            if (metadata_ == NULL)
                throw database_exception("No result data found.");

            columnCount_ = mysql_num_fields(metadata_);

            bindings_ = (MYSQL_BIND *) calloc(columnCount_, sizeof(MYSQL_BIND));

            auto fields = mysql_fetch_fields(metadata_);

            for (auto i = 0; i < columnCount_; i++)
            {
                switch (fields[i].type)
                {
                case MYSQL_TYPE_INT24:
                    bindings_[i].buffer_type = MYSQL_TYPE_LONGLONG;
                    break;
                case MYSQL_TYPE_DECIMAL:
                case MYSQL_TYPE_NEWDECIMAL:
                    bindings_[i].buffer_type = MYSQL_TYPE_DOUBLE;
                    break;
                case MYSQL_TYPE_BIT:
                    bindings_[i].buffer_type = MYSQL_TYPE_TINY;
                    break;
                case MYSQL_TYPE_YEAR:
                    break;
                case MYSQL_TYPE_VAR_STRING:
                    bindings_[i].buffer_type = MYSQL_TYPE_STRING;
                    break;
                case MYSQL_TYPE_SET:
                case MYSQL_TYPE_ENUM:
                case MYSQL_TYPE_GEOMETRY:
                    break;
                default:
                    bindings_[i].buffer_type = fields[i].type;
                    break;
                }
                bindings_[i].is_null = (my_bool *) calloc(1, sizeof(my_bool));
                bindings_[i].is_unsigned = 0;
                bindings_[i].error = 0;
                bindings_[i].buffer_length = fields[i].length;
                bindings_[i].length = (size_t *) calloc(1, sizeof(size_t));
                bindings_[i].buffer = calloc(1, fields[i].length);
            }

            if (mysql_stmt_bind_result(stmt_, bindings_) != 0)
            {
                throw database_exception(last_stmt_error(stmt_));
            }
        }

        bool mysql_stmt_resultset::is_valid() const
        {
            return stmt_ != NULL;
        }

        bool mysql_stmt_resultset::next()
        {
            if (stmt_ == NULL)
                return false;

            if (status_ == -1)
            {
                get_results();

                if (status_ == -1)
                    return false;
            }

            int res = mysql_stmt_fetch(stmt_);

            if (res == 1 || res == MYSQL_DATA_TRUNCATED)
                throw database_exception(last_stmt_error(stmt_));

            if (res == MYSQL_NO_DATA)
            {
                return false;
            }

            return true;

        }

        void mysql_stmt_resultset::reset()
        {

        }

        void print_binding(MYSQL_BIND *field)
        {
            switch (field->buffer_type)
            {
            case MYSQL_TYPE_TINY:
            case MYSQL_TYPE_SHORT:
            case MYSQL_TYPE_LONG:
                printf("int %d", *((int *) field->buffer));
                break;
            case MYSQL_TYPE_LONGLONG:
                printf("int64 %lld", *((int64_t *) field->buffer));
                break;
            case MYSQL_TYPE_FLOAT:
            case MYSQL_TYPE_DOUBLE:
                printf("float %f", *((double *) field->buffer));
                break;
            case MYSQL_TYPE_TIME:
            case MYSQL_TYPE_DATE:
            case MYSQL_TYPE_DATETIME:
            case  MYSQL_TYPE_TIMESTAMP:
                printf("time %ld", *((time_t *) field->buffer));
                break;
            case MYSQL_TYPE_STRING:
                printf("string %s", (char *) field->buffer);
                break;
            case MYSQL_TYPE_NULL:
                printf("null");
            default:
                printf("%p", field->buffer);
                break;
            }
            printf(" (%ld)\n", field->length ? *field->length : field->buffer_length);
        }

        row mysql_stmt_resultset::current_row()
        {
            return row(make_shared<mysql_stmt_row>(db_, metadata_, bindings_ ));
        }

        size_t mysql_stmt_resultset::column_count() const
        {
            return mysql_stmt_field_count(stmt_);
        }
    }
}
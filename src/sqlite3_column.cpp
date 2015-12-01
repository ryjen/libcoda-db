#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBSQLITE3

#include "sqlite3_column.h"

namespace arg3
{
    namespace db
    {
        sqlite3_column::sqlite3_column(shared_ptr<sqlite3_stmt> stmt, int column) : stmt_(stmt), column_(column)
        {
        }

        sqlite3_column::sqlite3_column(sqlite3_column &&other) : stmt_(other.stmt_), column_(other.column_)
        {
            other.stmt_ = nullptr;
        }

        sqlite3_column::~sqlite3_column()
        {
        }

        sqlite3_column &sqlite3_column::operator=(sqlite3_column &&other)
        {
            stmt_ = other.stmt_;

            other.stmt_ = nullptr;

            return *this;
        }

        bool sqlite3_column::is_valid() const
        {
            return stmt_ != nullptr;
        }

        void sqlite3_column::assert_value() const throw(no_such_column_exception)
        {
            if (!is_valid()) {
                throw no_such_column_exception();
            }
        }

        sql_value sqlite3_column::to_value() const
        {
            assert_value();

            switch (sqlite3_column_type(stmt_.get(), column_)) {
                case SQLITE_INTEGER:
                    return sqlite3_column_int64(stmt_.get(), column_);
                case SQLITE3_TEXT:
                default: {
                    const unsigned char *textValue = sqlite3_column_text(stmt_.get(), column_);
                    if (textValue != NULL) {
                        return reinterpret_cast<const char *>(textValue);
                    }
                    return sql_value();
                }
                case SQLITE_FLOAT:
                    return sqlite3_column_double(stmt_.get(), column_);
                case SQLITE_BLOB:
                    return sql_blob(sqlite3_column_blob(stmt_.get(), column_), sqlite3_column_bytes(stmt_.get(), column_));
            }
        }

        int sqlite3_column::sql_type() const
        {
            assert_value();

            return sqlite3_column_type(stmt_.get(), column_);
        }

        string sqlite3_column::name() const
        {
            return sqlite3_column_name(stmt_.get(), column_);
        }


        /* cached version */

        sqlite3_cached_column::sqlite3_cached_column(shared_ptr<sqlite3_stmt> stmt, int column)
        {
            name_ = sqlite3_column_name(stmt.get(), column);
            type_ = sqlite3_column_type(stmt.get(), column);

            switch (sqlite3_column_type(stmt.get(), column)) {
                case SQLITE_INTEGER:
                    value_ = sqlite3_column_int64(stmt.get(), column);
                    break;
                case SQLITE_TEXT:
                default: {
                    const unsigned char *textValue = sqlite3_column_text(stmt.get(), column);
                    if (textValue != NULL) {
                        value_ = reinterpret_cast<const char *>(textValue);
                    }
                    break;
                }
                case SQLITE_FLOAT:
                    value_ = sqlite3_column_double(stmt.get(), column);
                    break;
                case SQLITE_BLOB:
                    value_ = sql_blob(sqlite3_column_blob(stmt.get(), column), sqlite3_column_bytes(stmt.get(), column), NULL);
                    break;
            }
        }


        sqlite3_cached_column::sqlite3_cached_column(sqlite3_cached_column &&other)
            : name_(std::move(other.name_)), value_(std::move(other.value_)), type_(other.type_)
        {
        }

        sqlite3_cached_column::~sqlite3_cached_column()
        {
        }

        sqlite3_cached_column &sqlite3_cached_column::operator=(sqlite3_cached_column &&other)
        {
            name_ = std::move(other.name_);
            type_ = other.type_;
            value_ = std::move(other.value_);

            return *this;
        }

        bool sqlite3_cached_column::is_valid() const
        {
            return true;
        }

        sql_value sqlite3_cached_column::to_value() const
        {
            return value_;
        }

        int sqlite3_cached_column::sql_type() const
        {
            return type_;
        }

        string sqlite3_cached_column::name() const
        {
            return name_;
        }
    }
}

#endif

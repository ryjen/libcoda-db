#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBSQLITE3

#include "column.h"

namespace arg3
{
    namespace db
    {
        namespace sqlite_data_mapper
        {
            sql_value to_value(const shared_ptr<sqlite3_stmt> &stmt, int column)
            {
                switch (sqlite3_column_type(stmt.get(), column)) {
                    case SQLITE_INTEGER:
                        return sqlite3_column_int64(stmt.get(), column);
                    case SQLITE3_TEXT:
                    default: {
                        const unsigned char *textValue = sqlite3_column_text(stmt.get(), column);
                        if (textValue != NULL) {
                            return reinterpret_cast<const char *>(textValue);
                        }
                        return sql_value();
                    }
                    case SQLITE_FLOAT:
                        return sqlite3_column_double(stmt.get(), column);
                    case SQLITE_BLOB:
                        return sql_blob(sqlite3_column_blob(stmt.get(), column), sqlite3_column_bytes(stmt.get(), column));
                }
            }
        }

        sqlite3_column::sqlite3_column(const shared_ptr<sqlite3_stmt> &stmt, int column) : stmt_(stmt), column_(column)
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
            column_ = other.column_;

            other.stmt_ = nullptr;

            return *this;
        }

        bool sqlite3_column::is_valid() const
        {
            return stmt_ != nullptr;
        }

        sql_value sqlite3_column::to_value() const
        {
            if (!is_valid()) {
                throw no_such_column_exception();
            }

            return sqlite_data_mapper::to_value(stmt_, column_);
        }

        int sqlite3_column::sql_type() const
        {
            if (!is_valid()) {
                throw no_such_column_exception();
            }
            return sqlite3_column_type(stmt_.get(), column_);
        }

        string sqlite3_column::name() const
        {
            return sqlite3_column_name(stmt_.get(), column_);
        }


        /* cached version */

        sqlite3_cached_column::sqlite3_cached_column(shared_ptr<sqlite3_stmt> stmt, int column)
        {
            set_value(stmt, column);
        }

        void sqlite3_cached_column::set_value(shared_ptr<sqlite3_stmt> stmt, int column)
        {
            if (stmt == nullptr) {
                throw database_exception("no statement provided to sqlite3 column");
            }

            name_ = sqlite3_column_name(stmt.get(), column);
            type_ = sqlite3_column_type(stmt.get(), column);
            value_ = sqlite_data_mapper::to_value(stmt, column);
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

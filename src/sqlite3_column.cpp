#include "config.h"

#ifdef HAVE_LIBSQLITE3

#include "sqlite3_column.h"

namespace arg3
{
    namespace db
    {

        sqlite3_column::sqlite3_column(sqlite3_stmt *stmt, int column) : stmt_(stmt), column_(column)
        {
        }

        sqlite3_column::sqlite3_column(sqlite3_column &&other) : stmt_(other.stmt_), column_(other.column_) {}

        sqlite3_column::~sqlite3_column() {}

        sqlite3_column &sqlite3_column::operator=(sqlite3_column && other)
        {
            stmt_ = other.stmt_;

            return *this;
        }

        bool sqlite3_column::is_valid() const
        {
            return stmt_ != NULL;
        }

        void sqlite3_column::assert_value() const throw (no_such_column_exception)
        {
            if (stmt_ == NULL)
            {
                throw no_such_column_exception();
            }
        }

        sql_blob sqlite3_column::to_blob() const
        {
            assert_value();

            return sql_blob(sqlite3_column_blob(stmt_, column_), sqlite3_column_bytes(stmt_, column_), NULL);
        }

        double sqlite3_column::to_double() const
        {
            assert_value();

            return sqlite3_column_double(stmt_, column_);
        }
        bool sqlite3_column::to_bool() const
        {
            assert_value();

            return sqlite3_column_int(stmt_, column_);
        }
        int sqlite3_column::to_int() const
        {
            assert_value();

            return sqlite3_column_int(stmt_, column_);
        }

        int64_t sqlite3_column::to_int64() const
        {
            assert_value();

            return sqlite3_column_int64(stmt_, column_);
        }

        const unsigned char *sqlite3_column::to_text() const
        {
            assert_value();

            return sqlite3_column_text(stmt_, column_);
        }

        const wchar_t *sqlite3_column::to_text16() const
        {
            assert_value();
            return static_cast<const wchar_t *>(sqlite3_column_text16(stmt_, column_));
        }

        sql_value sqlite3_column::to_value() const
        {
            assert_value();

            switch (sqlite3_column_type(stmt_, column_))
            {
            case SQLITE_INTEGER:
                return sql_value(to_int64());
            case SQLITE_TEXT:
            default:
                return sql_value(to_string());
            case SQLITE_FLOAT:
                return sql_value(to_double());
            case SQLITE_BLOB:
                return sql_value(to_blob());
            }
        }

        int sqlite3_column::type() const
        {
            assert_value();

            return sqlite3_column_type(stmt_, column_);
        }

        string sqlite3_column::to_string() const
        {
            assert_value();

            const unsigned char *textValue = sqlite3_column_text(stmt_, column_);

            if (textValue == NULL)
                return string();

            return reinterpret_cast<const char *>(textValue);
        }

        string sqlite3_column::name() const
        {
            return sqlite3_column_name(stmt_, column_);
        }


        /* cached version */

        sqlite3_cached_column::sqlite3_cached_column(sqlite3_stmt *stmt, int column)
        {
            name_ = sqlite3_column_name(stmt, column);
            type_ = sqlite3_column_type(stmt, column);

            switch (sqlite3_column_type(stmt, column))
            {
            case SQLITE_INTEGER:
                value_ = sqlite3_column_int64(stmt, column);
                break;
            case SQLITE_TEXT:
            default:
            {
                const unsigned char *textValue = sqlite3_column_text(stmt, column);
                if (textValue != NULL)
                    value_ = std::string(reinterpret_cast<const char *>(textValue));
                break;
            }
            case SQLITE_FLOAT:
                value_ = sqlite3_column_double(stmt, column);
                break;
            case SQLITE_BLOB:
                value_ = sql_blob(sqlite3_column_blob(stmt, column), sqlite3_column_bytes(stmt, column), NULL);
                break;
            }
        }


        sqlite3_cached_column::sqlite3_cached_column(sqlite3_cached_column &&other) : name_(std::move(other.name_)),
            value_(std::move(other.value_)), type_(other.type_)
        {}

        sqlite3_cached_column::~sqlite3_cached_column() {}

        sqlite3_cached_column &sqlite3_cached_column::operator=(sqlite3_cached_column && other)
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

        sql_blob sqlite3_cached_column::to_blob() const
        {
            return value_;
        }

        double sqlite3_cached_column::to_double() const
        {
            return value_;
        }
        bool sqlite3_cached_column::to_bool() const
        {
            return value_;
        }
        int sqlite3_cached_column::to_int() const
        {
            return value_;
        }

        int64_t sqlite3_cached_column::to_int64() const
        {
            return value_;
        }

        sql_value sqlite3_cached_column::to_value() const
        {
            return value_;
        }

        int sqlite3_cached_column::type() const
        {
            return type_;
        }

        string sqlite3_cached_column::to_string() const
        {
            return value_;
        }

        string sqlite3_cached_column::name() const
        {
            return name_;
        }
    }
}

#endif

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

    }
}

#endif

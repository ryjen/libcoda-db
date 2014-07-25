#include "config.h"

#ifdef HAVE_LIBSQLITE3

#include "sqlite3_column.h"

namespace arg3
{
    namespace db
    {

        sqlite3_column::sqlite3_column(sqlite3_value *pValue) : value_(pValue)
        {
        }

        sqlite3_column::sqlite3_column(sqlite3_column &&other) : value_(other.value_) {}

        sqlite3_column::~sqlite3_column() {}

        sqlite3_column &sqlite3_column::operator=(sqlite3_column && other)
        {
            value_ = other.value_;

            return *this;
        }

        bool sqlite3_column::is_valid() const
        {
            return value_ != NULL;
        }

        void sqlite3_column::assert_value() const throw (no_such_column_exception)
        {
            if (value_ == NULL)
            {
                throw no_such_column_exception();
            }
        }

        sql_blob sqlite3_column::to_blob() const
        {
            assert_value();

            return sql_blob(sqlite3_value_blob(value_), sqlite3_value_bytes(value_), NULL);
        }

        double sqlite3_column::to_double() const
        {
            assert_value();

            return sqlite3_value_double(value_);
        }
        bool sqlite3_column::to_bool() const
        {
            assert_value();

            return sqlite3_value_int(value_);
        }
        int sqlite3_column::to_int() const
        {
            assert_value();

            return sqlite3_value_int(value_);
        }

        int64_t sqlite3_column::to_int64() const
        {
            assert_value();

            return sqlite3_value_int64(value_);
        }

        const unsigned char *sqlite3_column::to_text() const
        {
            assert_value();

            return sqlite3_value_text(value_);
        }

        const wchar_t *sqlite3_column::to_text16() const
        {
            assert_value();
            return static_cast<const wchar_t *>(sqlite3_value_text16(value_));
        }

        sql_value sqlite3_column::to_value() const
        {
            assert_value();

            switch (sqlite3_value_type(value_))
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

            return sqlite3_value_type(value_);
        }

        int sqlite3_column::numeric_type() const
        {
            assert_value();

            return sqlite3_value_numeric_type(value_);
        }

        sqlite3_column::operator sqlite3_value *() const
        {
            return value_;
        }

        string sqlite3_column::to_string() const
        {
            assert_value();

            const unsigned char *textValue = sqlite3_value_text(value_);

            if (textValue == NULL)
                return string();

            return reinterpret_cast<const char *>(textValue);
        }

    }
}

#endif

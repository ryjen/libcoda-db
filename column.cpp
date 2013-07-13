#include "column.h"

namespace arg3
{
    namespace db
    {
        column::column() : value_(NULL)
        {

        }
        column::column(sqlite3_value *pValue) : value_(pValue)
        {
        }

        column::column(const column &other) : value_(other.value_) {}

        column::column(column &&other) : value_(std::move(other.value_)) {}

        column::~column() {}

        column &column::operator=(const column &other)
        {
            if (this != &other)
            {
                value_ = other.value_;
            }
            return *this;
        }

        column &column::operator=(column &&other)
        {
            if (this != &other)
            {
                value_ = std::move(other.value_);
            }
            return *this;
        }

        bool column::is_valid() const
        {
            return value_ != NULL;
        }

        void column::assert_value() const throw (no_such_column_exception)
        {
            if (value_ == NULL)
            {
                throw no_such_column_exception();
            }
        }

        sql_blob column::to_blob() const
        {
            assert_value();

            return sql_blob(sqlite3_value_blob(value_), sqlite3_value_bytes(value_));
        }

        double column::to_double() const
        {
            assert_value();

            return sqlite3_value_double(value_);
        }

        int column::to_int() const
        {
            assert_value();

            return sqlite3_value_int(value_);
        }

        bool column::to_bool() const
        {
            assert_value();

            return sqlite3_value_int(value_);
        }

        sqlite3_int64 column::to_int64() const
        {
            assert_value();

            return sqlite3_value_int64(value_);
        }

        const unsigned char *column::to_text() const
        {
            assert_value();

            return sqlite3_value_text(value_);
        }

        string column::to_string() const
        {
            assert_value();

            const unsigned char *textValue = sqlite3_value_text(value_);

            if (textValue == NULL)
                return string();

            return reinterpret_cast<const char *>(textValue);
        }

        sql_value column::to_value() const
        {
            assert_value();

            switch(sqlite3_value_type(value_))
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

        int column::type() const
        {
            assert_value();

            return sqlite3_value_type(value_);
        }

        int column::numeric_type() const
        {
            assert_value();

            return sqlite3_value_numeric_type(value_);
        }

        column::operator sqlite3_value *() const
        {
            return value_;
        }

        column::operator string() const
        {
            return to_string();
        }

        column::operator int() const
        {
            return to_int();
        }

        column::operator sqlite3_int64() const
        {
            return to_int64();
        }

        column::operator double() const
        {
            return to_double();
        }
    }
}
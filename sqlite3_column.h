#ifndef ARG3_DB_SQLITE_COLUMN_H_
#define ARG3_DB_SQLITE_COLUMN_H_

#include "column.h"

namespace arg3
{
    namespace db
    {

        /*!
         * a sqlite specific implementation of a column
         */
        class sqlite3_column : public column_impl
        {
        private:
            sqlite3_value *value_;
            void assert_value() const throw (no_such_column_exception);
        public:

            sqlite3_column(sqlite3_value *pValue);
            sqlite3_column(const sqlite3_column &other);
            sqlite3_column(sqlite3_column &&other);
            virtual ~sqlite3_column();
            sqlite3_column &operator=(const sqlite3_column &other);
            sqlite3_column &operator=(sqlite3_column && other);

            bool is_valid() const;

            sql_blob to_blob() const;

            double to_double() const;

            int to_int() const;

            bool to_bool() const;

            int64_t to_int64() const;

            const unsigned char *to_text() const;

            string to_string() const;

            const wchar_t *to_text16() const;

            sql_value to_value() const;

            int type() const;

            operator sqlite3_value *() const;

            int numeric_type() const;
        };

    }
}

#endif

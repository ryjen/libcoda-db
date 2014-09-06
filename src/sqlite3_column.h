#ifndef ARG3_DB_SQLITE_COLUMN_H_
#define ARG3_DB_SQLITE_COLUMN_H_

#include "config.h"

#ifdef HAVE_LIBSQLITE3

#include "column.h"

#define sqlite3_default_column sqlite3_cached_column

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
            shared_ptr<sqlite3_stmt> stmt_;
            int column_;
            void assert_value() const throw (no_such_column_exception);
        public:

            sqlite3_column(shared_ptr<sqlite3_stmt> stmt, int column);
            sqlite3_column(const sqlite3_column &other) = delete;
            sqlite3_column(sqlite3_column &&other);
            virtual ~sqlite3_column();
            sqlite3_column &operator=(const sqlite3_column &other) = delete;
            sqlite3_column &operator=(sqlite3_column && other);

            bool is_valid() const;

            sql_blob to_blob() const;

            double to_double() const;

            int to_int() const;

            bool to_bool() const;

            long long to_llong() const;

            const unsigned char *to_text() const;

            string to_string() const;

            const wchar_t *to_text16() const;

            sql_value to_value() const;

            int type() const;

            string name() const;
        };

        /*!
         * a column that contains pre-fetched values
         */
        class sqlite3_cached_column : public column_impl
        {
        private:
            string name_;
            sql_value value_;
            int type_;
        public:

            sqlite3_cached_column(shared_ptr<sqlite3_stmt> stmt, int column);
            sqlite3_cached_column(const sqlite3_cached_column &other) = delete;
            sqlite3_cached_column(sqlite3_cached_column &&other);
            virtual ~sqlite3_cached_column();
            sqlite3_cached_column &operator=(const sqlite3_cached_column &other) = delete;
            sqlite3_cached_column &operator=(sqlite3_cached_column && other);

            bool is_valid() const;

            sql_blob to_blob() const;

            double to_double() const;

            int to_int() const;

            bool to_bool() const;

            long long to_llong() const;

            const unsigned char *to_text() const;

            string to_string() const;

            const wchar_t *to_text16() const;

            sql_value to_value() const;

            int type() const;

            string name() const;
        };
    }
}

#endif

#endif

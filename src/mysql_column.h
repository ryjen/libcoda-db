#ifndef ARG3_DB_MYSQL_COLUMN_H_
#define ARG3_DB_MYSQL_COLUMN_H_

#include "config.h"

#ifdef HAVE_LIBMYSQLCLIENT

#include <mysql/mysql.h>
#include "column.h"

namespace arg3
{
    namespace db
    {
        class mysql_binding;

        /*!
         * a mysql specific implementation of a column
         */
        class mysql_column : public column_impl
        {
        private:
            string name_;
            MYSQL_ROW value_;
            shared_ptr<MYSQL_RES> res_;
            size_t index_;
        public:

            mysql_column(shared_ptr<MYSQL_RES> res, MYSQL_ROW pValue, size_t index);
            mysql_column(const mysql_column &other);
            mysql_column(mysql_column &&other);
            virtual ~mysql_column();
            mysql_column &operator=(const mysql_column &other);
            mysql_column &operator=(mysql_column && other);

            bool is_valid() const;

            sql_blob to_blob() const;

            double to_double() const;

            int to_int() const;

            bool to_bool() const;

            int64_t to_int64() const;

            string to_string() const;

            sql_value to_value() const;

            int type() const;

            string name() const;

        };

        /*!
         * a mysql specific version of a column using prepared statements
         */
        class mysql_stmt_column : public column_impl
        {
        private:
            string name_;
            shared_ptr<mysql_binding> value_;
        public:

            mysql_stmt_column(const string &name, shared_ptr<mysql_binding> value);
            mysql_stmt_column(const mysql_stmt_column &other);
            mysql_stmt_column(mysql_stmt_column &&other);
            virtual ~mysql_stmt_column();
            mysql_stmt_column &operator=(const mysql_stmt_column &other);
            mysql_stmt_column &operator=(mysql_stmt_column && other);

            bool is_valid() const;

            sql_blob to_blob() const;

            double to_double() const;

            int to_int() const;

            bool to_bool() const;

            int64_t to_int64() const;

            string to_string() const;

            sql_value to_value() const;

            int type() const;

            string name() const;

        };

        class mysql_cached_column : public column_impl
        {
        private:
            string name_;
            sql_value value_;
            int type_;
        public:
            mysql_cached_column(const string &name, shared_ptr<mysql_binding> value);
            mysql_cached_column(MYSQL_RES *res, MYSQL_ROW pValue, size_t index);
            mysql_cached_column(const mysql_cached_column &other) = default;
            mysql_cached_column(mysql_cached_column &&other) = default;
            virtual ~mysql_cached_column() = default;
            mysql_cached_column &operator=(const mysql_cached_column &other) = default;
            mysql_cached_column &operator=(mysql_cached_column && other) = default;

            bool is_valid() const;

            sql_blob to_blob() const;

            double to_double() const;

            int to_int() const;

            bool to_bool() const;

            int64_t to_int64() const;

            string to_string() const;

            sql_value to_value() const;

            int type() const;

            string name() const;
        };
    }
}

#endif

#endif

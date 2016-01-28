#ifndef ARG3_DB_MYSQL_COLUMN_H
#define ARG3_DB_MYSQL_COLUMN_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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
            mysql_column(const shared_ptr<MYSQL_RES> &res, MYSQL_ROW pValue, size_t index);
            mysql_column(const mysql_column &other);
            mysql_column(mysql_column &&other);
            virtual ~mysql_column();
            mysql_column &operator=(const mysql_column &other);
            mysql_column &operator=(mysql_column &&other);

            bool is_valid() const;

            sql_value to_value() const;

            int sql_type() const;

            std::string name() const;
        };

        /*!
         * a mysql specific version of a column using prepared statements
         */
        class mysql_stmt_column : public column_impl
        {
           private:
            std::string name_;
            std::shared_ptr<mysql_binding> value_;
            size_t position_;

           public:
            mysql_stmt_column(const std::string &name, const shared_ptr<mysql_binding> &bindings, size_t position);
            mysql_stmt_column(const mysql_stmt_column &other);
            mysql_stmt_column(mysql_stmt_column &&other);
            virtual ~mysql_stmt_column();
            mysql_stmt_column &operator=(const mysql_stmt_column &other);
            mysql_stmt_column &operator=(mysql_stmt_column &&other);

            bool is_valid() const;

            sql_value to_value() const;

            int sql_type() const;

            std::string name() const;
        };

        /*!
         * a column that contains a prefetched value
         */
        class mysql_cached_column : public column_impl
        {
           private:
            std::string name_;
            sql_value value_;
            int type_;

            void set_value(shared_ptr<MYSQL_RES> res, MYSQL_ROW pValue, size_t index);

           public:
            mysql_cached_column(const std::string &name, mysql_binding &bindings, size_t position);
            mysql_cached_column(std::shared_ptr<MYSQL_RES> res, MYSQL_ROW pValue, size_t index);
            mysql_cached_column(const mysql_cached_column &other);
            mysql_cached_column(mysql_cached_column &&other);
            virtual ~mysql_cached_column();
            mysql_cached_column &operator=(const mysql_cached_column &other);
            mysql_cached_column &operator=(mysql_cached_column &&other);

            bool is_valid() const;

            sql_value to_value() const;

            int sql_type() const;

            std::string name() const;
        };
    }
}

#endif

#endif

/*!
 * @file mysql_column.h
 * a mysql specific representation of a column
 */
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
         * a mysql specific implementation of a column using fetching
         */
        class mysql_column : public column_impl
        {
           private:
            string name_;
            MYSQL_ROW value_;
            shared_ptr<MYSQL_RES> res_;
            size_t index_;

           public:
            /*!
             * @param res the result for column meta data
             * @param value the column values for the result
             * @param index the index of the column value
             */
            mysql_column(const shared_ptr<MYSQL_RES> &res, MYSQL_ROW value, size_t index);

            /* non-copyable boilerplate */
            mysql_column(const mysql_column &other) = delete;
            mysql_column(mysql_column &&other);
            virtual ~mysql_column();
            mysql_column &operator=(const mysql_column &other) = delete;
            mysql_column &operator=(mysql_column &&other);

            /* column_impl overrides */
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
            /*!
             * @param name the name of the column
             * @param bindings the bindings containing the column value
             * @param position the index of the column in the bindings
             */
            mysql_stmt_column(const std::string &name, const shared_ptr<mysql_binding> &bindings, size_t position);

            /* non-copyable boilerplate */
            mysql_stmt_column(const mysql_stmt_column &other) = delete;
            mysql_stmt_column(mysql_stmt_column &&other);
            virtual ~mysql_stmt_column();
            mysql_stmt_column &operator=(const mysql_stmt_column &other) = delete;
            mysql_stmt_column &operator=(mysql_stmt_column &&other);

            /* column_impl overrides */
            bool is_valid() const;
            sql_value to_value() const;
            int sql_type() const;
            std::string name() const;
        };
    }
}

#endif

#endif

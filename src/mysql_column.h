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

            /* boilerplate */
            mysql_column(const mysql_column &other);
            mysql_column(mysql_column &&other);
            virtual ~mysql_column();
            mysql_column &operator=(const mysql_column &other);
            mysql_column &operator=(mysql_column &&other);

            /*!
             * @return true if the column internals are open
             */
            bool is_valid() const;

            /*!
             * @return the coverted value of the column
             */
            sql_value to_value() const;

            /*!
             * @return the type of the column value
             * NOTE: is a mysql specific type value
             */
            int sql_type() const;

            /*!
             * @return the name of the column
             */
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

            /* boilerplate */
            mysql_stmt_column(const mysql_stmt_column &other);
            mysql_stmt_column(mysql_stmt_column &&other);
            virtual ~mysql_stmt_column();
            mysql_stmt_column &operator=(const mysql_stmt_column &other);
            mysql_stmt_column &operator=(mysql_stmt_column &&other);

            /*!
             * @return true if the internals are open and valid
             */
            bool is_valid() const;

            /*!
             * @return the converted value of the column
             */
            sql_value to_value() const;

            /*!
             * @return the type of value for this column
             * NOTE: is a mysql specific type value
             */
            int sql_type() const;

            /*!
             * @return the name of this column
             */
            std::string name() const;
        };

        /*!
         * a column that contains a prefetched value with no dependencies on an open db connection
         * @see caching types in sqldb
         */
        class mysql_cached_column : public column_impl
        {
           private:
            std::string name_;
            sql_value value_;
            int type_;

            void set_value(const shared_ptr<MYSQL_RES> &res, MYSQL_ROW pValue, size_t index);

           public:
            /*!
             * @param name the name of the column
             * @param bindings the values for the row columns
             * @param position the index of the column in the bindings
             */
            mysql_cached_column(const std::string &name, mysql_binding &bindings, size_t position);

            /*!
             * @param res the result for the column meta data
             * @param value the values for the result
             * @param index the index of the value
             */
            mysql_cached_column(const std::shared_ptr<MYSQL_RES> &res, MYSQL_ROW value, size_t index);

            /* boilerplate */
            mysql_cached_column(const mysql_cached_column &other);
            mysql_cached_column(mysql_cached_column &&other);
            virtual ~mysql_cached_column();
            mysql_cached_column &operator=(const mysql_cached_column &other);
            mysql_cached_column &operator=(mysql_cached_column &&other);

            /*!
             * @return true if the db internals are open and valid
             */
            bool is_valid() const;

            /*!
             * @return the converted value of this column
             */
            sql_value to_value() const;

            /*!
             * @return the type of value of this column
             * NOTE: this is a mysql specific type value
             */
            int sql_type() const;

            /*!
             * @return the name of this column
             */
            std::string name() const;
        };
    }
}

#endif

#endif

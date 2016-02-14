/*!
 * @file column.h
 */
#ifndef ARG3_DB_SQLITE_COLUMN_H
#define ARG3_DB_SQLITE_COLUMN_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBSQLITE3

#include <sqlite3.h>
#include "../column.h"

namespace arg3
{
    namespace db
    {
        namespace sqlite
        {
            /*!
             * a sqlite specific implementation of a column
             */
            class column : public column_impl
            {
               private:
                std::shared_ptr<sqlite3_stmt> stmt_;
                int column_;

               public:
                /*!
                 * @param stmt      the statement in use
                 * @param column    the column index
                 */
                column(const std::shared_ptr<sqlite3_stmt> &stmt, int column);

                /* non-copyable boilerplate */
                column(const column &other) = delete;
                column(column &&other);
                virtual ~column();
                column &operator=(const column &other) = delete;
                column &operator=(column &&other);

                /* column_impl overrides */
                bool is_valid() const;
                sql_value to_value() const;
                int sql_type() const;
                std::string name() const;
            };

            /*!
             * a column that contains pre-fetched values
             */
            class cached_column : public column_impl
            {
               private:
                std::string name_;
                sql_value value_;
                int type_;
                void set_value(const std::shared_ptr<sqlite3_stmt> &stmt, int column);

               public:
                /*!
                 * @param stmt    the sqlite3 statement in use
                 * @param column  the column index
                 */
                cached_column(const std::shared_ptr<sqlite3_stmt> &stmt, int column);

                /* non-copyable boilerplate */
                cached_column(const cached_column &other) = delete;
                cached_column(cached_column &&other);
                virtual ~cached_column();
                cached_column &operator=(const cached_column &other) = delete;
                cached_column &operator=(cached_column &&other);

                /* column_impl overrides */
                bool is_valid() const;
                sql_value to_value() const;
                int sql_type() const;
                std::string name() const;
            };
        }
    }
}

#endif

#endif

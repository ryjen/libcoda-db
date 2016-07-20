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
        }
    }
}

#endif

#endif

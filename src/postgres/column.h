/*!
 * @file column.h
 * column in a postgres database
 */
#ifndef ARG3_DB_POSTGRES_COLUMN_H
#define ARG3_DB_POSTGRES_COLUMN_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include <libpq-fe.h>
#include "../column.h"

namespace arg3
{
    namespace db
    {
        namespace postgres
        {
            /*!
             * a sqlite specific implementation of a column
             */
            class column : public column_impl
            {
               private:
                std::shared_ptr<PGresult> stmt_;
                int column_;
                int row_;

               public:
                /*!
                 * @param stmt the query statement in use
                 * @param row the row index
                 * @param column the column index
                 */
                column(const std::shared_ptr<PGresult> &stmt, int row, int column);

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

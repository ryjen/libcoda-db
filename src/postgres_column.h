/*!
 * @file postgres_column.h
 * column in a postgres database
 */
#ifndef ARG3_DB_POSTGRES_COLUMN_H
#define ARG3_DB_POSTGRES_COLUMN_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include <libpq-fe.h>
#include "column.h"

namespace arg3
{
    namespace db
    {
        /*!
         * a sqlite specific implementation of a column
         */
        class postgres_column : public column_impl
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
            postgres_column(const std::shared_ptr<PGresult> &stmt, int row, int column);

            /* non-copyable boilerplate */
            postgres_column(const postgres_column &other) = delete;
            postgres_column(postgres_column &&other);
            virtual ~postgres_column();
            postgres_column &operator=(const postgres_column &other) = delete;
            postgres_column &operator=(postgres_column &&other);

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

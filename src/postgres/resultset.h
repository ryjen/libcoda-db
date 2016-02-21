/*!
 * @file resultset.h
 * a postgres result set
 */
#ifndef ARG3_DB_POSTGRES_RESULTSET_H
#define ARG3_DB_POSTGRES_RESULTSET_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include <vector>
#include <libpq-fe.h>
#include "../resultset.h"

namespace arg3
{
    namespace db
    {
        namespace postgres
        {
            class db;

            /*!
             * a postgres specific implmentation of a result set
             */
            class resultset : public resultset_impl
            {
                friend class row;
                friend class sqldb;
                template <typename, typename>
                friend class resultset_iterator;

               private:
                std::shared_ptr<PGresult> stmt_;
                postgres::db *db_;
                int currentRow_;

               public:
                /*!
                 * @param  db    the database in use
                 * @param  stmt  the query result in use
                 */
                resultset(postgres::db *db, const std::shared_ptr<PGresult> &stmt);

                /* non-copyable boilerplate */
                resultset(const resultset &other) = delete;
                resultset(resultset &&other);
                virtual ~resultset();
                resultset &operator=(const resultset &other) = delete;
                resultset &operator=(resultset &&other);

                /* resultset_impl overrides */
                bool is_valid() const;
                row_type current_row();
                void reset();
                bool next();
                size_t size() const;
            };
        }
    }
}

#endif

#endif

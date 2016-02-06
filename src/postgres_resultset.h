/*!
 * @file postgres_resultset.h
 * a postgres result set
 */
#ifndef ARG3_DB_POSTGRES_RESULTSET_H
#define ARG3_DB_POSTGRES_RESULTSET_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include <libpq-fe.h>
#include "resultset.h"
#include <vector>

namespace arg3
{
    namespace db
    {
        class postgres_db;

        /*!
         * a postgres specific implmentation of a result set
         */
        class postgres_resultset : public resultset_impl
        {
            friend class select_query;
            friend class row;
            friend class sqldb;
            template <typename, typename>
            friend class resultset_iterator;

           private:
            std::shared_ptr<PGresult> stmt_;
            postgres_db *db_;
            int currentRow_;

           public:
            /*!
             * @param  db    the database in use
             * @param  stmt  the query result in use
             */
            postgres_resultset(postgres_db *db, const std::shared_ptr<PGresult> &stmt);

            /* non-copyable boilerplate */
            postgres_resultset(const postgres_resultset &other) = delete;
            postgres_resultset(postgres_resultset &&other);
            virtual ~postgres_resultset();
            postgres_resultset &operator=(const postgres_resultset &other) = delete;
            postgres_resultset &operator=(postgres_resultset &&other);

            /* resultset_impl overrides */
            bool is_valid() const;
            row current_row();
            void reset();
            bool next();
            size_t size() const;
        };
    }
}

#endif

#endif

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
         * a sqlite specific implmentation of a result set
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
            postgres_resultset(postgres_db *db, std::shared_ptr<PGresult> stmt);
            postgres_resultset(const postgres_resultset &other) = delete;
            postgres_resultset(postgres_resultset &&other);
            virtual ~postgres_resultset();

            postgres_resultset &operator=(const postgres_resultset &other) = delete;
            postgres_resultset &operator=(postgres_resultset &&other);

            bool is_valid() const;

            row current_row();

            void reset();

            bool next();

            size_t size() const;
        };

        /*!
         * a resultset that contains pre-fetched rows
         */
        class postgres_cached_resultset : public resultset_impl
        {
            friend class select_query;
            friend class row;
            friend class sqldb;
            template <typename, typename>
            friend class resultset_iterator;

           private:
            postgres_db *db_;
            std::vector<std::shared_ptr<row_impl>> rows_;
            int currentRow_;

           public:
            postgres_cached_resultset(postgres_db *db, std::shared_ptr<PGresult> stmt);
            postgres_cached_resultset(const postgres_cached_resultset &other) = delete;
            postgres_cached_resultset(postgres_cached_resultset &&other);
            virtual ~postgres_cached_resultset();

            postgres_cached_resultset &operator=(const postgres_cached_resultset &other) = delete;
            postgres_cached_resultset &operator=(postgres_cached_resultset &&other);

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

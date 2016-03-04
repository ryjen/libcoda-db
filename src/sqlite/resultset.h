/*!
 * @file resultset.h
 */
#ifndef ARG3_DB_SQLITE_RESULTSET_H
#define ARG3_DB_SQLITE_RESULTSET_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBSQLITE3

#include <vector>
#include <sqlite3.h>
#include "../resultset.h"

namespace arg3
{
    namespace db
    {
        namespace sqlite
        {
            class db;

            /*!
             * a sqlite specific implmentation of a result set
             */
            class resultset : public resultset_impl
            {
                template <typename, typename>
                friend class resultset_iterator;

               private:
                std::shared_ptr<sqlite3_stmt> stmt_;
                sqlite::db *db_;
                int status_;

               public:
                /*!
                 * @param  db   the database in use
                 * @param  stmt the query statement in use
                 */
                resultset(sqlite::db *db, const std::shared_ptr<sqlite3_stmt> &stmt);

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

            /*!
             * a resultset that contains pre-fetched rows
             */
            class cached_resultset : public resultset_impl
            {
                template <typename, typename>
                friend class resultset_iterator;

               private:
                sqlite::db *db_;
                std::vector<std::shared_ptr<row_impl>> rows_;
                int currentRow_;

               public:
                /*!
                 * @param db    the database in use
                 * @param stmt  the statement in use
                 */
                cached_resultset(sqlite::db *db, std::shared_ptr<sqlite3_stmt> stmt);

                /* non-copyable boilerplate */
                cached_resultset(const cached_resultset &other) = delete;
                cached_resultset(cached_resultset &&other);
                virtual ~cached_resultset();
                cached_resultset &operator=(const cached_resultset &other) = delete;
                cached_resultset &operator=(cached_resultset &&other);

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

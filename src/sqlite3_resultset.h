#ifndef ARG3_DB_SQLITE_RESULTSET_H
#define ARG3_DB_SQLITE_RESULTSET_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBSQLITE3

#include <sqlite3.h>
#include "resultset.h"
#include <vector>

namespace arg3
{
    namespace db
    {
        class sqlite3_db;

        /*!
         * a sqlite specific implmentation of a result set
         */
        class sqlite3_resultset : public resultset_impl
        {
            friend class select_query;
            friend class row;
            friend class sqldb;
            template <typename, typename>
            friend class resultset_iterator;

           private:
            std::shared_ptr<sqlite3_stmt> stmt_;
            sqlite3_db *db_;
            int status_;

           public:
            sqlite3_resultset(sqlite3_db *db, const std::shared_ptr<sqlite3_stmt> &stmt);
            sqlite3_resultset(const sqlite3_resultset &other) = delete;
            sqlite3_resultset(sqlite3_resultset &&other);
            virtual ~sqlite3_resultset();

            sqlite3_resultset &operator=(const sqlite3_resultset &other) = delete;
            sqlite3_resultset &operator=(sqlite3_resultset &&other);

            bool is_valid() const;

            row current_row();

            void reset();

            bool next();

            size_t size() const;
        };

        /*!
         * a resultset that contains pre-fetched rows
         */
        class sqlite3_cached_resultset : public resultset_impl
        {
            friend class select_query;
            friend class row;
            friend class sqldb;
            template <typename, typename>
            friend class resultset_iterator;

           private:
            sqlite3_db *db_;
            std::vector<std::shared_ptr<row_impl>> rows_;
            int currentRow_;

           public:
            sqlite3_cached_resultset(sqlite3_db *db, shared_ptr<sqlite3_stmt> stmt);
            sqlite3_cached_resultset(const sqlite3_cached_resultset &other) = delete;
            sqlite3_cached_resultset(sqlite3_cached_resultset &&other);
            virtual ~sqlite3_cached_resultset();

            sqlite3_cached_resultset &operator=(const sqlite3_cached_resultset &other) = delete;
            sqlite3_cached_resultset &operator=(sqlite3_cached_resultset &&other);

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

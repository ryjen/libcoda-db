/*!
 * @file sqlite3_row.h
 */
#ifndef ARG3_DB_SQLITE_ROW_H
#define ARG3_DB_SQLITE_ROW_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBSQLITE3

#include "row.h"
#include "sqlite3_column.h"
#include <vector>

namespace arg3
{
    namespace db
    {
        class sqlite3_db;

        /*!
         *  a sqlite specific implementation of a row
         */
        class sqlite3_row : public row_impl
        {
            friend class sqlite3_resultset;

           private:
            std::shared_ptr<sqlite3_stmt> stmt_;
            sqlite3_db *db_;
            size_t size_;

           public:
            /*!
             * @param db    the database in use
             * @param stmt  the query statement in use
             */
            sqlite3_row(sqlite3_db *db, const std::shared_ptr<sqlite3_stmt> &stmt);

            /* non-copyable boilerplate */
            virtual ~sqlite3_row();
            sqlite3_row(const sqlite3_row &other) = delete;
            sqlite3_row(sqlite3_row &&other);
            sqlite3_row &operator=(const sqlite3_row &other) = delete;
            sqlite3_row &operator=(sqlite3_row &&other);

            /* row_impl overrides */
            std::string column_name(size_t nPosition) const;
            column_type column(size_t nPosition) const;
            column_type column(const std::string &name) const;
            size_t size() const;
            bool is_valid() const;
        };

        /*!
         * a row that contains pre-fetched columns
         */
        class sqlite3_cached_row : public row_impl
        {
            friend class sqlite3_resultset;

           private:
            std::vector<std::shared_ptr<sqlite3_cached_column>> columns_;

           public:
            /*!
             * @param db    the database in use
             * @param stmt  the query statement in use
             */
            sqlite3_cached_row(sqlite3_db *db, std::shared_ptr<sqlite3_stmt> stmt);

            /* non-copyable boilerplate */
            virtual ~sqlite3_cached_row();
            sqlite3_cached_row(const sqlite3_cached_row &other) = delete;
            sqlite3_cached_row(sqlite3_cached_row &&other);
            sqlite3_cached_row &operator=(const sqlite3_cached_row &other) = delete;
            sqlite3_cached_row &operator=(sqlite3_cached_row &&other);

            /* row_impl overrides */
            std::string column_name(size_t nPosition) const;
            column_type column(size_t nPosition) const;
            column_type column(const std::string &name) const;
            size_t size() const;
            bool is_valid() const;
        };
    }
}

#endif

#endif

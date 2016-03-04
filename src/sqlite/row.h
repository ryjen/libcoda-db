/*!
 * @file row.h
 */
#ifndef ARG3_DB_SQLITE_ROW_H
#define ARG3_DB_SQLITE_ROW_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBSQLITE3

#include <vector>
#include "../row.h"
#include "column.h"

namespace arg3
{
    namespace db
    {
        namespace sqlite
        {
            class db;

            /*!
             *  a sqlite specific implementation of a row
             */
            class row : public row_impl
            {
               private:
                std::shared_ptr<sqlite3_stmt> stmt_;
                sqlite::db *db_;
                size_t size_;

               public:
                /*!
                 * @param db    the database in use
                 * @param stmt  the query statement in use
                 */
                row(sqlite::db *db, const std::shared_ptr<sqlite3_stmt> &stmt);

                /* non-copyable boilerplate */
                virtual ~row();
                row(const row &other) = delete;
                row(row &&other);
                row &operator=(const row &other) = delete;
                row &operator=(row &&other);

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
            class cached_row : public row_impl
            {
               private:
                std::vector<std::shared_ptr<cached_column>> columns_;

               public:
                /*!
                 * @param db    the database in use
                 * @param stmt  the query statement in use
                 */
                cached_row(sqlite::db *db, std::shared_ptr<sqlite3_stmt> stmt);

                /* non-copyable boilerplate */
                virtual ~cached_row();
                cached_row(const cached_row &other) = delete;
                cached_row(cached_row &&other);
                cached_row &operator=(const cached_row &other) = delete;
                cached_row &operator=(cached_row &&other);

                /* row_impl overrides */
                std::string column_name(size_t nPosition) const;
                column_type column(size_t nPosition) const;
                column_type column(const std::string &name) const;
                size_t size() const;
                bool is_valid() const;
            };
        }
    }
}

#endif

#endif

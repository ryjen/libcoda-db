/*!
 * @file row.h
 */
#ifndef RJ_DB_SQLITE_ROW_H
#define RJ_DB_SQLITE_ROW_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBSQLITE3

#include <vector>
#include "../row.h"
#include "column.h"

namespace rj
{
    namespace db
    {
        namespace sqlite
        {
            class session;

            /*!
             *  a sqlite specific implementation of a row
             */
            class row : public row_impl
            {
               private:
                std::shared_ptr<sqlite3_stmt> stmt_;
                std::shared_ptr<sqlite::session> sess_;
                size_t size_;

               public:
                /*!
                 * @param db    the database in use
                 * @param stmt  the query statement in use
                 */
                row(const std::shared_ptr<sqlite::session> &sess, const std::shared_ptr<sqlite3_stmt> &stmt);

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
        }
    }
}

#endif

#endif

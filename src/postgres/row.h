/*!
 * @file row.h
 * A postgres row
 */
#ifndef ARG3_DB_POSTGRES_ROW_H
#define ARG3_DB_POSTGRES_ROW_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include <vector>
#include "../row.h"
#include "column.h"

namespace arg3
{
    namespace db
    {
        namespace postgres
        {
            class session;

            /*!
             *  a sqlite specific implementation of a row
             */
            class row : public row_impl
            {
               private:
                std::shared_ptr<PGresult> stmt_;
                std::shared_ptr<postgres::session> sess_;
                size_t size_;
                int row_;

               public:
                /*!
                 * @param db    the database in use
                 * @param stmt  the query statement result in use
                 * @param row   the row index
                 */
                row(const std::shared_ptr<postgres::session> &sess, const std::shared_ptr<PGresult> &stmt, int row);

                /* non-copyable boilerplate */
                virtual ~row();
                row(const row &other) = delete;
                row(row &&other);
                row &operator=(const row &other) = delete;
                row &operator=(row &&other);

                /*! row_impl overrides */
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

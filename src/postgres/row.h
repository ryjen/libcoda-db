/*!
 * @file row.h
 * A postgres row
 */
#ifndef RJ_DB_POSTGRES_ROW_H
#define RJ_DB_POSTGRES_ROW_H

#include <libpq-fe.h>
#include <vector>
#include "../row.h"

namespace rj
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

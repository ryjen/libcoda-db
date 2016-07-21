/*!
 * @file resultset.h
 * a postgres result set
 */
#ifndef RJ_DB_POSTGRES_RESULTSET_H
#define RJ_DB_POSTGRES_RESULTSET_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include <libpq-fe.h>
#include <vector>
#include "../resultset.h"

namespace rj
{
    namespace db
    {
        namespace postgres
        {
            class session;

            /*!
             * a postgres specific implmentation of a result set
             */
            class resultset : public resultset_impl
            {
                template <typename, typename>
                friend class resultset_iterator;

               private:
                std::shared_ptr<PGresult> stmt_;
                std::shared_ptr<postgres::session> sess_;
                int currentRow_;

               public:
                /*!
                 * @param  db    the database in use
                 * @param  stmt  the query result in use
                 */
                resultset(const std::shared_ptr<postgres::session> &sess, const std::shared_ptr<PGresult> &stmt);

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
            };
        }
    }
}

#endif

#endif

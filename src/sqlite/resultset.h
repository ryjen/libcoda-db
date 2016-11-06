/*!
 * @file resultset.h
 */
#ifndef RJ_DB_SQLITE_RESULTSET_H
#define RJ_DB_SQLITE_RESULTSET_H

#include <sqlite3.h>
#include <vector>
#include "../resultset.h"

namespace rj
{
    namespace db
    {
        namespace sqlite
        {
            class session;

            /*!
             * a sqlite specific implmentation of a result set
             */
            class resultset : public resultset_impl
            {
                template <typename, typename>
                friend class resultset_iterator;

               private:
                std::shared_ptr<sqlite3_stmt> stmt_;
                std::shared_ptr<sqlite::session> sess_;
                int status_;

               public:
                /*!
                 * @param  db   the database in use
                 * @param  stmt the query statement in use
                 */
                resultset(const std::shared_ptr<sqlite::session> &sess, const std::shared_ptr<sqlite3_stmt> &stmt);

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

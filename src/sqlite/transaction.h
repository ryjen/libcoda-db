#ifndef ARG3_DB_SQLITE_TRANSACTION_H
#define ARG3_DB_SQLITE_TRANSACTION_H

#ifdef HAVE_LIBSQLITE3

#include <sqlite3.h>
#include "../transaction.h"

namespace arg3
{
    namespace db
    {
        namespace sqlite
        {
            class transaction : public arg3::db::transaction_impl
            {
               public:
                typedef enum { none, deferred, immediate, exclusive } type;

                transaction(const std::shared_ptr<sqlite3> &db, transaction::type type = none);
                void start();
                bool is_active() const;

               private:
                std::shared_ptr<sqlite3> db_;
                transaction::type type_;
            };
        }
    }
}
#endif

#endif

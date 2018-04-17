#ifndef CODA_DB_SQLITE_TRANSACTION_H
#define CODA_DB_SQLITE_TRANSACTION_H

#include <sqlite3.h>
#include "../transaction.h"

namespace coda
{
    namespace db
    {
        namespace sqlite
        {
            class transaction : public coda::db::transaction_impl
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

#ifndef RJ_DB_POSTGRES_TRANSACTION_H
#define RJ_DB_POSTGRES_TRANSACTION_H

#ifdef HAVE_LIBPQ

#include <libpq-fe.h>
#include "../transaction.h"

namespace rj
{
    namespace db
    {
        namespace postgres
        {
            class transaction : public transaction_impl
            {
               public:
                typedef struct {
                    isolation::level isolation;
                    rj::db::transaction::type type;
                    int deferrable;
                } mode;

                transaction(const std::shared_ptr<PGconn> &db, const transaction::mode &mode = {});

                void start();
                bool is_active() const;

               private:
                std::shared_ptr<PGconn> db_;
                transaction::mode mode_;
            };
        }
    }
}

#endif

#endif

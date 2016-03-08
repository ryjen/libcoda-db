#ifndef ARG3_DB_POSTGRES_TRANSACTION_H
#define ARG3_DB_POSTGRES_TRANSACTION_H

#ifdef HAVE_LIBPQ

#include <libpq-fe.h>
#include "../transaction.h"

namespace arg3
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
                    arg3::db::transaction::type type;
                    int deferrable;
                } mode;

                transaction(const std::shared_ptr<PGconn> &db, const transaction::mode &mode = {});

                void start();
                void commit();
                void rollback();
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

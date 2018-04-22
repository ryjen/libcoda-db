#ifndef CODA_DB_POSTGRES_TRANSACTION_H
#define CODA_DB_POSTGRES_TRANSACTION_H

#include <libpq-fe.h>
#include "../transaction.h"

namespace coda
{
    namespace db
    {
        namespace postgres
        {
            class transaction : public transaction_impl
            {
               public:
                typedef struct {
                    coda::db::transaction::isolation::level isolation;
                    coda::db::transaction::type type;
                    int deferrable;
                } mode;

                transaction(const std::shared_ptr<PGconn> &db, const transaction::mode &mode = {});

                void start();
                bool is_active() const noexcept;

               private:
                std::shared_ptr<PGconn> db_;
                transaction::mode mode_;
            };
        }
    }
}

#endif

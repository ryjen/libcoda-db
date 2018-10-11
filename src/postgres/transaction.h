#ifndef CODA_DB_POSTGRES_TRANSACTION_H
#define CODA_DB_POSTGRES_TRANSACTION_H

#include <libpq-fe.h>
#include "../transaction.h"

namespace coda::db::postgres {
      class transaction : public transaction_impl {
       public:
        typedef struct {
          coda::db::transaction::isolation::level isolation;
          coda::db::transaction::type type;
          int deferrable;
        } mode;

        explicit transaction(const std::shared_ptr<PGconn> &db, const transaction::mode &mode = {});

        void start() override;
        bool is_active() const noexcept override;

       private:
        std::shared_ptr<PGconn> db_;
        transaction::mode mode_;
      };
}  // namespace coda::db::postgres

#endif

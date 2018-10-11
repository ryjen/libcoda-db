#ifndef CODA_DB_SQLITE_TRANSACTION_H
#define CODA_DB_SQLITE_TRANSACTION_H

#include <sqlite3.h>
#include "../transaction.h"

namespace coda::db::sqlite {
  class transaction : public coda::db::transaction_impl {
   public:
    typedef enum { none, deferred, immediate, exclusive } type;

    explicit transaction(const std::shared_ptr<sqlite3> &db, transaction::type type = none);
    void start() override;
    bool is_active() const noexcept override;

   private:
    std::shared_ptr<sqlite3> db_;
    transaction::type type_;
  };
}  // namespace coda::db::sqlite

#endif

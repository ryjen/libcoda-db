#ifndef CODA_DB_MYSQL_TRANSACTION_H
#define CODA_DB_MYSQL_TRANSACTION_H

#include <mysql/mysql.h>
#include "../transaction.h"

namespace coda::db::mysql {
      class transaction : public transaction_impl {
        friend class session;

       public:
        typedef struct {
          db::transaction::type type;
          db::transaction::isolation::level isolation;
        } mode;

        explicit transaction(const std::shared_ptr<MYSQL> &db, const transaction::mode &mode = {});
        transaction(const transaction &other) = default;
        transaction(transaction &&other) noexcept = default;
        ~transaction() override = default;
        transaction &operator=(const transaction &other) = default;
        transaction &operator=(transaction &&other) noexcept = default;

        void start() override;
        bool is_active() const noexcept override;

       private:
        std::shared_ptr<MYSQL> db_;
        transaction::mode mode_;
        bool active_;
      };
}  // namespace coda::db::mysql

#endif

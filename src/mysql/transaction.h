#ifndef CODA_DB_MYSQL_TRANSACTION_H
#define CODA_DB_MYSQL_TRANSACTION_H

#include "../transaction.h"
#include <mysql/mysql.h>

namespace coda {
  namespace db {
    namespace mysql {
      class transaction : public transaction_impl {
        friend class session;

        public:
        typedef struct {
          db::transaction::type type;
          db::transaction::isolation::level isolation;
        } mode;

        transaction(const std::shared_ptr<MYSQL> &db,
                    const transaction::mode &mode = {});
        transaction(const transaction &other);
        transaction(transaction &&other);
        virtual ~transaction();
        transaction &operator=(const transaction &other);
        transaction &operator=(transaction &&other);

        void start();
        bool is_active() const noexcept;

        private:
        std::shared_ptr<MYSQL> db_;
        transaction::mode mode_;
        bool active_;
      };
    } // namespace mysql
  }   // namespace db
} // namespace coda

#endif

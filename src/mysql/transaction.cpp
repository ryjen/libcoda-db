
#include "transaction.h"
#include "../exception.h"

namespace coda::db::mysql {
      transaction::transaction(const std::shared_ptr<MYSQL> &db, const transaction::mode &mode)
          : db_(db), mode_(mode), active_(false) {}

      bool transaction::is_active() const noexcept { return active_; }

      void transaction::start() {
        std::string buf = "START TRANSACTION";

        switch (mode_.type) {
          default:
          case db::transaction::none:
            break;
          case db::transaction::read_write:
            buf += " READ WRITE";
            break;
          case db::transaction::read_only:
            buf += " READ ONLY";
            break;
        }

        buf += ";";

        if (mysql_query(db_.get(), buf.c_str())) {
          throw transaction_exception(std::string("unable to start transaction: ") + mysql_error(db_.get()));
        }

        active_ = true;
      }
}  // namespace coda::db:mysql

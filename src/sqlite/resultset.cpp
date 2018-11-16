#include "resultset.h"
#include "../exception.h"
#include "row.h"
#include "session.h"

using namespace std;

namespace coda::db::sqlite {
      resultset::resultset(const std::shared_ptr<sqlite::session> &sess, const shared_ptr<sqlite3_stmt> &stmt)
          : stmt_(stmt), sess_(sess), status_(-1) {
        if (sess_ == nullptr) {
          throw database_exception("No database provided to sqlite3 resultset");
        }

        if (stmt_ == nullptr) {
          throw database_exception("no statement provided to sqlite3 resultset");
        }
      }

      bool resultset::is_valid() const noexcept { return stmt_ != nullptr && stmt_; }

      bool resultset::next() {
        if (!is_valid()) {
          return false;
        }

        if (status_ == SQLITE_DONE) {
          return false;
        }

        status_ = sqlite3_step(stmt_.get());

        return status_ == SQLITE_ROW;
      }

      void resultset::reset() {
        if (!is_valid()) {
          // resultset::reset database not open
          return;
        }

        if (sqlite3_reset(stmt_.get()) != SQLITE_OK) {
          throw database_exception(sess_->last_error());
        }
        status_ = -1;
      }

      resultset::row_type resultset::current_row() { return row_type(make_shared<row>(sess_, stmt_)); }
}  // namespace coda::db::sqlite

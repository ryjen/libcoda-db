
#include "resultset.h"
#include "binding.h"
#include "row.h"
#include "session.h"

using namespace std;

namespace coda::db::mysql {
  namespace helper {
    extern string last_stmt_error(MYSQL_STMT *stmt);

    /**
     * @cond internal
     * @param p the result
     */
    void res_delete::operator()(MYSQL_RES *p) const {
      if (p != nullptr) {
        mysql_free_result(p);
      }
    }
  }  // namespace helper

  resultset::resultset(const std::shared_ptr<mysql::session> &sess, const shared_ptr<MYSQL_RES> &res)
      : res_(res), row_(nullptr), sess_(sess) {
    if (sess_ == nullptr) {
      throw database_exception("database not provided to mysql resultset");
    }
  }

  bool resultset::is_valid() const noexcept { return res_ != nullptr; }

  bool resultset::next() {
    if (sess_ == nullptr) {
      // mysql resultset next: database not open
      return false;
    }

    if (!is_valid() || !sess_->is_open()) {
      return false;
    }

    row_ = mysql_fetch_row(res_.get());

    return row_ != nullptr;
  }

  void resultset::reset() {
    if (res_ != nullptr) {
      mysql_data_seek(res_.get(), 0);
    }
  }

  resultset::row_type resultset::current_row() { return row_type(make_shared<mysql::row>(sess_, res_, row_)); }
  /* Statement version */

  stmt_resultset::stmt_resultset(const std::shared_ptr<mysql::session> &sess, const shared_ptr<MYSQL_STMT> &stmt)
      : stmt_(stmt), metadata_(nullptr), sess_(sess), bindings_(nullptr), status_(-1) {
    if (stmt_ == nullptr) {
      throw database_exception("invalid statement provided to mysql statement resultset");
    }
    if (sess_ == nullptr) {
      throw database_exception("invalid database provided to mysql statement resultset");
    }
  }

  void stmt_resultset::prepare_results() {
    if (stmt_ == nullptr || !stmt_ || status_ != INVALID) {
      return;
    }

    if ((status_ = mysql_stmt_execute(stmt_.get()))) {
      throw database_exception(helper::last_stmt_error(stmt_.get()));
    }

    // get information about the results
    MYSQL_RES *temp = mysql_stmt_result_metadata(stmt_.get());

    if (temp == nullptr) {
      throw database_exception("No result data found.");
    }

    metadata_ = shared_ptr<MYSQL_RES>(temp, helper::res_delete());

    int size = mysql_num_fields(temp);

    auto fields = mysql_fetch_fields(temp);

    bindings_ = make_shared<mysql::binding>(fields, size);

    bindings_->bind_result(stmt_.get());
  }

  bool stmt_resultset::is_valid() const noexcept { return stmt_ != nullptr && stmt_; }

  bool stmt_resultset::next() {
    if (!is_valid()) {
      // mysql resultset next invalid
      return false;
    }

    if (status_ == INVALID) {
      prepare_results();

      if (status_ == INVALID) {
        return false;
      }
    }

    int res = mysql_stmt_fetch(stmt_.get());

    if (res == 1 || res == MYSQL_DATA_TRUNCATED) {
      throw database_exception(helper::last_stmt_error(stmt_.get()));
    }

    return res != MYSQL_NO_DATA;

  }

  void stmt_resultset::reset() {
    bindings_.reset();

    if (!is_valid()) {
      // mysql stmt resultset reset invalid
      return;
    }

    status_ = INVALID;

    if (mysql_stmt_reset(stmt_.get())) {
      throw database_exception(helper::last_stmt_error(stmt_.get()));
    }
  }

  resultset::row_type stmt_resultset::current_row() {
    return row_type(make_shared<stmt_row>(sess_, stmt_, metadata_, bindings_));
  }
}  // namespace coda::db::mysql

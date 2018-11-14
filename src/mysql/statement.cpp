
#include "statement.h"
#include "../exception.h"
#include "resultset.h"
#include "session.h"
#include <string>

using namespace std;

namespace coda {
  namespace db {
    namespace mysql {
      namespace helper {
        extern string last_stmt_error(MYSQL_STMT *stmt);

        struct stmt_delete {
          void operator()(MYSQL_STMT *p) const {
            if (p != nullptr) {
              mysql_stmt_close(p);
            }
          }
        };
      } // namespace helper

      statement::statement(const std::shared_ptr<session> &sess)
          : sess_(sess), stmt_(nullptr) {
        if (sess_ == nullptr) {
          throw database_exception("No database provided for mysql statement");
        }
      }

      statement::statement(statement &&other)
          : sess_(std::move(other.sess_)), stmt_(std::move(other.stmt_)) {
        other.sess_ = nullptr;
        other.stmt_ = nullptr;
      }

      statement &statement::operator=(statement &&other) {
        sess_ = std::move(other.sess_);
        stmt_ = std::move(other.stmt_);

        other.sess_ = nullptr;
        other.stmt_ = nullptr;

        return *this;
      }

      statement::~statement() { finish(); }

      void statement::prepare(const string &sql) {
        if (sess_ == nullptr || !sess_->is_open()) {
          throw database_exception("database is not open");
        }

        MYSQL_STMT *temp = mysql_stmt_init(sess_->db_.get());

        if (temp == nullptr) {
          throw database_exception("out of memory");
        }

        stmt_ = shared_ptr<MYSQL_STMT>(temp, helper::stmt_delete());

        string formatted_sql = bindings_.prepare(sql);

        if (mysql_stmt_prepare(stmt_.get(), formatted_sql.c_str(),
                               formatted_sql.length())) {
          throw database_exception(sess_->last_error());
        }
      }

      bool statement::is_valid() const noexcept {
        return stmt_ != nullptr && stmt_;
      }

      /**
       * binding methods ensure the dynamic array is sized properly and store
       * the value as a memory pointer
       */
      statement &statement::bind(size_t index, const sql_value &value) {
        bindings_.bind(index, value);
        return *this;
      }

      statement &statement::bind(const string &name, const sql_value &value) {
        bindings_.bind(name, value);
        return *this;
      }

      statement::resultset_type statement::results() {
        if (!is_valid()) {
          throw database_exception("statement not ready");
        }

        bindings_.bind_params(stmt_.get());

        return resultset_type(make_shared<stmt_resultset>(sess_, stmt_));
      }

      bool statement::result() {
        if (!is_valid()) {
          // mysql statement result invalid
          return false;
        }

        bindings_.bind_params(stmt_.get());

        if (mysql_stmt_execute(stmt_.get())) {
          return false;
        }
        return true;
      }

      int statement::last_number_of_changes() {
        if (!is_valid()) {
          // mysql statement result invalid
          return 0;
        }

        return mysql_stmt_affected_rows(stmt_.get());
      }

      string statement::last_error() {
        if (!is_valid()) {
          throw database_exception("statement not ready");
        }

        return helper::last_stmt_error(stmt_.get());
      }

      void statement::finish() {
        bindings_.reset();

        if (stmt_ != nullptr) {
          mysql_stmt_free_result(stmt_.get());
          stmt_ = nullptr;
        }
      }

      void statement::reset() {
        if (!is_valid()) {
          // mysql statement reset invalid
          return;
        }

        if (mysql_stmt_reset(stmt_.get())) {
          throw database_exception(last_error());
        }
      }

      long long statement::last_insert_id() {
        if (!is_valid()) {
          return 0;
        }

        return mysql_stmt_insert_id(stmt_.get());
      }

      size_t statement::num_of_bindings() const noexcept {
        return bindings_.num_of_bindings();
      }
    } // namespace mysql
  }   // namespace db
} // namespace coda

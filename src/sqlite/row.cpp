#include "row.h"
#include "../exception.h"
#include "column.h"
#include "session.h"

using namespace std;

namespace coda::db::sqlite {
      row::row(const std::shared_ptr<sqlite::session> &sess, const shared_ptr<sqlite3_stmt> &stmt)
          : row_impl(), stmt_(stmt), sess_(sess) {
        if (sess_ == nullptr) {
          throw database_exception("no database provided to sqlite3 row");
        }

        if (stmt_ == nullptr) {
          throw database_exception("no statement provided to sqlite3 row");
        }

        size_ = static_cast<size_t>(sqlite3_column_count(stmt_.get()));
      }

      row::column_type row::column(size_t nPosition) const {
        if (nPosition >= size()) {
          throw no_such_column_exception();
        }

        return column_type(make_shared<sqlite::column>(stmt_, nPosition));
      }

      row::column_type row::column(const string &name) const {
        if (name.empty()) {
          throw no_such_column_exception();
        }

        for (size_t i = 0; i < size_; i++) {
          const char *col_name = sqlite3_column_name(stmt_.get(), i);

          if (name == col_name) {
            return column(i);
          }
        }
        throw no_such_column_exception(name);
      }

      string row::column_name(size_t nPosition) const {
        if (nPosition >= size()) {
          throw no_such_column_exception();
        }

        return sqlite3_column_name(stmt_.get(), static_cast<int>(nPosition));
      }

      size_t row::size() const noexcept { return size_; }

      bool row::is_valid() const noexcept { return stmt_ != nullptr; }
}  // namespace coda::db::sqlite

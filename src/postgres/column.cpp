
#include "column.h"
#include "../sql_value.h"
#include "binding.h"

using namespace std;

namespace coda::db::postgres {
      column::column(const shared_ptr<PGresult> &stmt, int row, int column) : stmt_(stmt), column_(column), row_(row) {}

      bool column::is_valid() const { return stmt_ != nullptr && row_ >= 0 && column_ >= 0; }

      sql_value column::to_value() const {
        if (!is_valid()) {
          throw no_such_column_exception();
        }

        return data_mapper::to_value(PQftype(stmt_.get(), column_), PQgetvalue(stmt_.get(), row_, column_),
                                     PQgetlength(stmt_.get(), row_, column_));
      }

      int column::sql_type() const {
        if (!is_valid()) {
          throw no_such_column_exception();
        }
        return PQftype(stmt_.get(), column_);
      }

      string column::name() const {
        if (!is_valid()) {
          return string();
        }
        return PQfname(stmt_.get(), column_);
      }
}  // namespace coda::db::postgres

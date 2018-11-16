
#include "column.h"
#include "../sql_value.h"

using namespace std;

namespace coda::db::sqlite {
      namespace data_mapper {
        sql_value to_value(const shared_ptr<sqlite3_stmt> &stmt, int column) {
          if (stmt == nullptr || column < 0 || column >= sqlite3_column_count(stmt.get())) {
            return sql_value();
          }

          switch (sqlite3_column_type(stmt.get(), column)) {
            case SQLITE_INTEGER:
              return sql_number(sqlite3_column_int64(stmt.get(), column));
            case SQLITE3_TEXT:
            default: {
              const unsigned char *textValue = sqlite3_column_text(stmt.get(), column);
              if (textValue != nullptr) {
                return sql_string(reinterpret_cast<const char *>(textValue));
              }
              return sql_value();
            }
            case SQLITE_FLOAT:
              return sql_number(sqlite3_column_double(stmt.get(), column));
            case SQLITE_BLOB: {
              return sql_blob(sqlite3_column_blob(stmt.get(), column),
                              static_cast<size_t>(sqlite3_column_bytes(stmt.get(), column)));
            }
          }
        }
      }  // namespace data_mapper

      column::column(const shared_ptr<sqlite3_stmt> &stmt, int column) : stmt_(stmt), column_(column) {}

      bool column::is_valid() const { return stmt_ != nullptr && stmt_ && column_ >= 0; }

      sql_value column::to_value() const {
        if (!is_valid()) {
          throw no_such_column_exception();
        }

        return data_mapper::to_value(stmt_, column_);
      }

      int column::sql_type() const {
        if (!is_valid()) {
          throw no_such_column_exception();
        }
        return sqlite3_column_type(stmt_.get(), column_);
      }

      string column::name() const {
        if (!is_valid()) {
          return string();
        }
        return sqlite3_column_name(stmt_.get(), column_);
      }
}  // namespace coda::db::sqlite

#include "sql_generator.h"

namespace coda::db {
    std::string sql_generator::to_sql() const {
      if (!sql_.empty()) {
        return sql_;
      }

      return generate_sql();
    }

    std::string sql_generator::to_sql() {
      if (!sql_.empty()) {
        return sql_;
      }
      sql_ = generate_sql();
      return sql_;
    }

    void sql_generator::reset() { sql_.clear(); }
}  // namespace coda::db
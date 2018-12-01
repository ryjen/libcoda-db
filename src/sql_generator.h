#ifndef CODA_DB_SQL_GENERATOR_H
#define CODA_DB_SQL_GENERATOR_H

#include <string>

namespace coda::db {
  class sql_generator {
   public:
    std::string to_sql() const;
    std::string to_sql();

    virtual void reset();

   protected:
    virtual std::string generate_sql() const = 0;

   private:
    std::string sql_;
  };
}  // namespace coda::db
#endif

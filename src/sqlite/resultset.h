/*!
 * @file resultset.h
 */
#ifndef CODA_DB_SQLITE_RESULTSET_H
#define CODA_DB_SQLITE_RESULTSET_H

#include <sqlite3.h>
#include <vector>
#include "../resultset.h"

namespace coda::db::sqlite {
  class session;

  /*!
   * a sqlite specific implmentation of a result set
   */
  class resultset : public resultset_impl {

   private:
    std::shared_ptr<sqlite3_stmt> stmt_;
    std::shared_ptr<sqlite::session> sess_;
    int status_;

   public:
    /*!
     * @param  db   the database in use
     * @param  stmt the query statement in use
     */
    resultset(const std::shared_ptr<sqlite::session> &sess, const std::shared_ptr<sqlite3_stmt> &stmt);

    /* non-copyable boilerplate */
    resultset(const resultset &other) = delete;
    resultset(resultset &&other) noexcept = default;
    ~resultset() override = default;
    resultset &operator=(const resultset &other) = delete;
    resultset &operator=(resultset &&other) noexcept = default;

    /* resultset_impl overrides */
    bool is_valid() const noexcept override;
    row_type current_row() override;
    /**
     * @throws database_exception if unable to reset statement
     */
    void reset() override;
    bool next() override;
  };
}  // namespace coda::db::sqlite

#endif

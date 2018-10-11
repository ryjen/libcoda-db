/*!
 * @file row.h
 * A postgres row
 */
#ifndef CODA_DB_POSTGRES_ROW_H
#define CODA_DB_POSTGRES_ROW_H

#include <libpq-fe.h>
#include <vector>
#include "../row.h"

namespace coda::db::postgres {
  class session;

  /*!
   *  a sqlite specific implementation of a row
   */
  class row : public row_impl {
   private:
    std::shared_ptr<PGresult> stmt_;
    std::shared_ptr<postgres::session> sess_;
    size_t size_;
    int row_;

   public:
    /*!
     * @param db    the database in use
     * @param stmt  the query statement result in use
     * @param row   the row index
     */
    row(const std::shared_ptr<postgres::session> &sess, const std::shared_ptr<PGresult> &stmt, int row);

    /* non-copyable boilerplate */
    ~row() override = default;
    row(const row &other) = delete;
    row(row &&other) noexcept = default;
    row &operator=(const row &other) = delete;
    row &operator=(row &&other) noexcept = default;

    /*! row_impl overrides */
    std::string column_name(size_t nPosition) const override;
    column_type column(size_t nPosition) const override;
    column_type column(const std::string &name) const override;
    size_t size() const noexcept override;
    bool is_valid() const noexcept override;
  };
}  // namespace coda::db::postgres

#endif

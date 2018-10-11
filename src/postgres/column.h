/*!
 * @file column.h
 * column in a postgres database
 */
#ifndef CODA_DB_POSTGRES_COLUMN_H
#define CODA_DB_POSTGRES_COLUMN_H

#include <libpq-fe.h>
#include "../column.h"

namespace coda::db::postgres {
  /*!
   * a sqlite specific implementation of a column
   */
  class column : public column_impl {
   private:
    std::shared_ptr<PGresult> stmt_;
    int column_;
    int row_;

   public:
    /*!
     * @param stmt the query statement in use
     * @param row the row index
     * @param column the column index
     */
    column(const std::shared_ptr<PGresult> &stmt, int row, int column);

    /* non-copyable boilerplate */
    column(const column &other) = delete;
    column(column &&other) noexcept = default;
    ~column() = default;
    column &operator=(const column &other) = delete;
    column &operator=(column &&other) noexcept = default;

    /* column_impl overrides */
    bool is_valid() const override;
    sql_value to_value() const override;
    int sql_type() const;
    std::string name() const override;
  };
}  // namespace coda::db::postgres

#endif

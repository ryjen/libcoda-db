/*!
 * @file column.h
 */
#ifndef CODA_DB_SQLITE_COLUMN_H
#define CODA_DB_SQLITE_COLUMN_H

#include <sqlite3.h>
#include "../column.h"

namespace coda::db::sqlite {
  /*!
   * a sqlite specific implementation of a column
   */
  class column : public column_impl {
   private:
    std::shared_ptr<sqlite3_stmt> stmt_;
    int column_;

   public:
    /*!
     * @param stmt      the statement in use
     * @param column    the column index
     */
    column(const std::shared_ptr<sqlite3_stmt> &stmt, int column);

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
}  // namespace coda::db::sqlite

#endif

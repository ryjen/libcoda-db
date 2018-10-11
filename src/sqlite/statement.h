/*!
 * a sqlite3 sql statement
 * @file statement.h
 */
#ifndef CODA_DB_SQLITE_STATEMENT_H
#define CODA_DB_SQLITE_STATEMENT_H

#include <sqlite3.h>
#include "../statement.h"

namespace coda::db::sqlite {
  class session;

  /*!
   * a sqlite specific implementation of a statement
   */
  class statement : public coda::db::statement {
   private:
    std::shared_ptr<sqlite::session> sess_;
    std::shared_ptr<sqlite3_stmt> stmt_;
    size_t bound_;

   public:
    /*!
     * @param db    the database in use
     */
    explicit statement(const std::shared_ptr<sqlite::session> &sess);

    /* non-copyable boilerplate */
    statement(const statement &other) = delete;
    statement(statement &&other) noexcept = default;
    statement &operator=(const statement &other) = delete;
    statement &operator=(statement &&other) noexcept = default;
    ~statement() override = default;

    /* statement overrides */
    void prepare(const std::string &sql) override;
    bool is_valid() const noexcept override;
    resultset_type query() override;
    bool execute() override;
    void finish() override;
    void reset() override;
    unsigned long long last_number_of_changes() override;
    std::string last_error() override;
    long long last_insert_id() override;

    /* bindable overrides */
    statement &bind(size_t index, const sql_value &value) override;
    statement &bind(const std::string &name, const sql_value &value) override;
    size_t num_of_bindings() const noexcept override;
  };

  namespace helper {
    /*!
     * helper to cleanup a sqlite3 statement
     */
    struct stmt_delete {
      void operator()(sqlite3_stmt *p) const;
    };
  }  // namespace helper
}  // namespace coda::db::sqlite

#endif

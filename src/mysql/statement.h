/*!
 * @file statement.h
 * Mysql specific implementation of a query statement
 */
#ifndef CODA_DB_MYSQL_STATEMENT_H
#define CODA_DB_MYSQL_STATEMENT_H

#include <mysql/mysql.h>
#include "../statement.h"
#include "binding.h"

namespace coda::db::mysql {
  class session;

  /*!
   * a mysql specific implementation of a statement
   */
  class statement : public coda::db::statement {
   private:
    std::shared_ptr<session> sess_;
    std::shared_ptr<MYSQL_STMT> stmt_;
    binding bindings_;

   public:
    /*!
     * @param db the database in use
     */
    explicit statement(const std::shared_ptr<session> &sess);

    /* non-copyable boilerplate */
    statement(const statement &other) = delete;
    statement(statement &&other) = default;
    statement &operator=(const statement &other) = delete;
    statement &operator=(statement &&other) = default;
    ~statement() override;

    /* statement overrides */
    void prepare(const std::string &sql) override;
    bool is_valid() const noexcept override;
    resultset_type query() override;
    bool execute() override;
    void finish() override;
    /**
     * @throws database_exception if unable to reset statement
     */
    void reset() override;
    unsigned long long last_number_of_changes() override;
    long long last_insert_id() override;

    /**
     * @throws database_exeption if statement is invalid
     * @return the last error as a string
     */
    std::string last_error() override;

    /* bindable overrides */
    statement &bind(size_t index, const sql_value &value) override;
    statement &bind(const std::string &name, const sql_value &value) override;
    size_t num_of_bindings() const noexcept override;
  };
}  // namespace coda::db::mysql

#endif

/*!
 * @file resultset.h
 * representation of results from a mysql query
 */
#ifndef CODA_DB_MYSQL_RESULTSET_H
#define CODA_DB_MYSQL_RESULTSET_H

#include <mysql/mysql.h>
#include <vector>
#include "../resultset.h"

namespace coda::db::mysql {
  class session;
  class binding;

  /*!
   * a mysql specific implementation of a result set
   */
  class resultset : public resultset_impl {

   private:
    std::shared_ptr<MYSQL_RES> res_;
    MYSQL_ROW row_;
    std::shared_ptr<mysql::session> sess_;

   public:
    /*!
     * @param db the database in use
     * @param res the query results
     */
    resultset(const std::shared_ptr<mysql::session> &sess, const std::shared_ptr<MYSQL_RES> &res);

    /* non-copyable boilerplate */
    resultset(const resultset &other) = delete;
    resultset(resultset &&other) noexcept = default;
    resultset &operator=(const resultset &other) = delete;
    resultset &operator=(resultset &&other) noexcept = default;
    ~resultset() override = default;

    /* resultset_impl overrides */
    bool is_valid() const noexcept override;
    resultset::row_type current_row() override;
    void reset() override;
    bool next() override;
  };

  /*!
   * a mysql specific implementation of a result set using prepared
   * statements
   */
  class stmt_resultset : public resultset_impl {
   private:
    std::shared_ptr<MYSQL_STMT> stmt_;
    std::shared_ptr<MYSQL_RES> metadata_;
    std::shared_ptr<mysql::session> sess_;
    std::shared_ptr<mysql::binding> bindings_;
    int status_;
    void prepare_results();

    constexpr static const int INVALID = -1;

   public:
    /*!
     * @param db the database in use
     * @param stmt the statement being executed
     */
    stmt_resultset(const std::shared_ptr<mysql::session> &sess, const std::shared_ptr<MYSQL_STMT> &stmt);

    /* non-copyable boilerplate */
    stmt_resultset(const stmt_resultset &other) = delete;
    stmt_resultset &operator=(const stmt_resultset &other) = delete;
    stmt_resultset(stmt_resultset &&other) noexcept = default;
    stmt_resultset &operator=(stmt_resultset &&other) noexcept = default;

    /* boilerplate */
    ~stmt_resultset() override = default;

    /* resultset_impl overrides */
    bool is_valid() const noexcept override;
    stmt_resultset::row_type current_row() override;
    void reset() override;
    bool next() override;
  };

  namespace helper {
    /*! helper to clean up after a query result */
    struct res_delete {
      void operator()(MYSQL_RES *p) const;
    };
  }  // namespace helper
}  // namespace coda::db::mysql

#endif

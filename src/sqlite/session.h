/*!
 * @file session.h
 */
#ifndef CODA_DB_SQLITE_SESSION_H
#define CODA_DB_SQLITE_SESSION_H

#include <sqlite3.h>
#include "../session.h"
#include "../session_factory.h"
#include "transaction.h"

namespace coda::db::sqlite {
  __attribute__((constructor)) void initialize();

  class factory : public session_factory {
   public:
    std::shared_ptr<coda::db::session_impl> create(const uri &uri) override;
  };

  /*!
   * a sqlite specific implementation of a database
   */
  class session : public coda::db::session_impl, public std::enable_shared_from_this<session> {
    friend class factory;
    friend class statement;

   protected:
    std::shared_ptr<sqlite3> db_;

   public:
    /*!
     * @param info   the connection info
     */
    explicit session(const uri &info);

    /* boilerplate */
    session(const session &other) = delete;
    session(session &&other) noexcept = default;
    session &operator=(const session &other) = delete;
    session &operator=(session &&other) noexcept = default;
    ~session();

    /* sql session overrides */
    bool is_open() const noexcept override;
    void open(int flags);
    void open() override;
    void close() override;
    long long last_insert_id() const override;
    unsigned long long last_number_of_changes() const override;
    std::shared_ptr<resultset_impl> query(const std::string &sql) override;
    bool execute(const std::string &sql) override;
    std::string last_error() const override;
    std::shared_ptr<statement_type> create_statement() override;
    std::shared_ptr<transaction_impl> create_transaction() const override;
    std::shared_ptr<transaction_impl> create_transaction(transaction::type type) const;

    /*! @copydoc
     *  overridden for sqlite3 specific pragma parsing
     */
    std::vector<column_definition> get_columns_for_schema(const std::string &dbName,
                                                          const std::string &tableName) override;
    std::string bind_param(size_t index) const override;

    [[nodiscard]] constexpr int features() const override;
  };
}  // namespace coda::db::sqlite

#endif

/*!
 * @file db.h
 * a mysql specific database
 */
#ifndef CODA_DB_MYSQL_SESSION_H
#define CODA_DB_MYSQL_SESSION_H

#include <mysql/mysql.h>
#include "../session.h"
#include "../session_factory.h"

namespace coda::db::mysql {
  __attribute__((constructor)) void initialize();

  class factory : public session_factory {
   public:
    std::shared_ptr<coda::db::session_impl> create(const uri &uri) override;
  };

  /*!
   * a mysql specific implementation of a database
   */
  class session : public coda::db::session_impl, public std::enable_shared_from_this<session> {
    friend class resultset;
    friend class statement;
    friend class factory;

   private:
    std::vector<std::string> get_primary_keys(const std::string &dbName, const std::string &tableName);

   protected:
    std::shared_ptr<MYSQL> db_;

   public:
    /*!
     * default constructor takes a uri to connect to
     * @param connInfo the uri connection info
     */
    explicit session(const uri &connInfo);

    /* boilerplate */
    session(const session &other) = delete;
    session(session &&other) noexcept = delete;
    session &operator=(const session &other) = delete;
    session &operator=(session &&other) noexcept = delete;
    ~session();

    /* sqldb overrides */
    bool is_open() const noexcept override;
    void open() override;
    void close() override;
    long long last_insert_id() const override;
    unsigned long long last_number_of_changes() const override;
    std::string last_error() const override;
    std::shared_ptr<resultset_impl> query(const std::string &sql) override;
    bool execute(const std::string &sql) override;
    std::shared_ptr<statement_type> create_statement() override;
    std::shared_ptr<transaction_impl> create_transaction() const override;
    std::vector<column_definition> get_columns_for_schema(const std::string &dbName,
                                                          const std::string &tablename) override;
    std::string bind_param(size_t index) const override;
    [[nodiscard]] constexpr int features() const override;
  };
}  // namespace coda::db::mysql

#endif

/*!
 * @file db.h
 * a postgres database
 */
#ifndef CODA_DB_POSTGRES_SESSION_H
#define CODA_DB_POSTGRES_SESSION_H

#include <libpq-fe.h>
#include "../session.h"
#include "../session_factory.h"
#include "transaction.h"

namespace coda::db::postgres {
      __attribute__((constructor)) void initialize();

      class factory : public session_factory {
       public:
        std::shared_ptr<coda::db::session_impl> create(const uri &uri) override;
      };

      /*!
       * a mysql specific implementation of a database
       */
      class session : public coda::db::session_impl, public std::enable_shared_from_this<session> {
        friend class statement;
        friend class factory;

       protected:
        std::shared_ptr<PGconn> db_;

       public:
        /*!
         * @param info the connection uri
         */
        explicit session(const uri &info);

        /* boilerplate */
        session(const session &other) = delete;
        session(session &&other) noexcept = default;
        session &operator=(const session &other) = delete;
        session &operator=(session &&other) noexcept = default;
        ~session();

        /* sql db overrides */
        bool is_open() const noexcept override;
        void open() override;
        void close() override;
        long long last_insert_id() const override;
        unsigned long long last_number_of_changes() const override;
        std::string last_error() const override;
        std::shared_ptr<resultset_impl> query(const std::string &sql) override;
        bool execute(const std::string &sql) override;
        std::shared_ptr<coda::db::session::statement_type> create_statement() override;
        std::shared_ptr<transaction_impl> create_transaction() const override;
        std::shared_ptr<transaction_impl> create_transaction(const transaction::mode &mode) const;
        std::vector<column_definition> get_columns_for_schema(const std::string &dbName, const std::string &tablename) override;
        std::string bind_param(size_t index) const override;
        [[nodiscard]] constexpr int features() const override;

       private:
        long long lastId_;
        unsigned long long lastNumChanges_;
        void set_last_insert_id(long long value);
        void set_last_number_of_changes(unsigned long long value);
      };
}  // namespace coda::db::postgres

#endif

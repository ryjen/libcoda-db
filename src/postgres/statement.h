/*!
 * @file statement.h
 * A postgres statement
 */
#ifndef CODA_DB_POSTGRES_STATEMENT_H
#define CODA_DB_POSTGRES_STATEMENT_H

#include <libpq-fe.h>
#include "../statement.h"
#include "binding.h"

namespace coda::db::postgres {
      class session;

      /*!
       * a sqlite specific implementation of a statement
       */
      class statement : public coda::db::statement {
       private:
        std::shared_ptr<postgres::session> sess_;
        std::shared_ptr<PGresult> stmt_;
        binding bindings_;
        std::string sql_;

       public:
        /*!
         * @param db    the database in use
         */
        explicit statement(const std::shared_ptr<postgres::session> &sess);

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

      /*
       * utility to cleanup a postgres result
       */
      namespace helper {
        struct res_delete {
          void operator()(PGresult *p) const;
        };
      }  // namespace helper
}  // namespace coda::db::postgres

#endif

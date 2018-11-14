/*!
 * @file statement.h
 * Mysql specific implementation of a query statement
 */
#ifndef CODA_DB_MYSQL_STATEMENT_H
#define CODA_DB_MYSQL_STATEMENT_H

#include "../statement.h"
#include "binding.h"
#include <mysql/mysql.h>

namespace coda {
  namespace db {
    namespace mysql {
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
        statement(const std::shared_ptr<session> &sess);

        /* non-copyable boilerplate */
        statement(const statement &other) = delete;
        statement(statement &&other);
        statement &operator=(const statement &other) = delete;
        statement &operator=(statement &&other);
        virtual ~statement();

        /* statement overrides */
        void prepare(const std::string &sql);
        bool is_valid() const noexcept;
        resultset_type results();
        bool result();
        void finish();
        /**
         * @throws database_exception if unable to reset statement
         */
        void reset();
        int last_number_of_changes();
        long long last_insert_id();

        /**
         * @throws database_exeption if statement is invalid
         * @return the last error as a string
         */
        std::string last_error();

        /* bindable overrides */
        statement &bind(size_t index, const sql_value &value);
        statement &bind(const std::string &name, const sql_value &value);
        size_t num_of_bindings() const noexcept;
      };
    } // namespace mysql
  }   // namespace db
} // namespace coda

#endif

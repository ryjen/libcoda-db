/*!
 * a sqlite3 sql statement
 * @file statement.h
 */
#ifndef CODA_DB_SQLITE_STATEMENT_H
#define CODA_DB_SQLITE_STATEMENT_H

#include "../statement.h"
#include <sqlite3.h>

namespace coda {
  namespace db {
    namespace sqlite {
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
        statement(const std::shared_ptr<sqlite::session> &sess);

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
        void reset();
        int last_number_of_changes();
        std::string last_error();
        long long last_insert_id();

        /* bindable overrides */
        statement &bind(size_t index, const sql_value &value);
        statement &bind(const std::string &name, const sql_value &value);
        size_t num_of_bindings() const noexcept;
      };

      namespace helper {
        /*!
         * helper to cleanup a sqlite3 statement
         */
        struct stmt_delete {
          void operator()(sqlite3_stmt *p) const;
        };
      } // namespace helper
    }   // namespace sqlite
  }     // namespace db
} // namespace coda

#endif

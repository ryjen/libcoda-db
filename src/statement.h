/*!
 * @file statement.h
 * an interface for a query statement
 */
#ifndef CODA_DB_STATEMENT_H
#define CODA_DB_STATEMENT_H

#include "bindable.h"
#include "resultset.h"
#include "sql_generator.h"

namespace coda::db {
    /*!
     * base class for a database specific implementation of a prepared statement
     */
    class statement : public bindable {
     public:
      using resultset_type = coda::db::resultset;

     private:
     public:
      statement() = default;

      statement(const statement &other) = default;

      statement(statement &&other) noexcept = default;

      ~statement() override = default;

      statement &operator=(const statement &other) = default;

      statement &operator=(statement &&other) noexcept = default;

      /*!
       * prepares this statement for execution
       * @param sql the sql to prepare
       */
      virtual void prepare(const std::string &sql) = 0;

      void prepare(const sql_generator &obj) { prepare(obj.to_sql()); }

      /*!
       * releases resources for this statement
       */
      virtual void finish() = 0;

      /*!
       * resets this statement for a new execution
       */
      virtual void reset() = 0;

      /*!
       * tests if this statement is valid
       * @return true if the internals are valid
       */
      virtual bool is_valid() const noexcept = 0;

      /*!
       * executes this statement
       * @return a set of the results
       */
      virtual resultset_type results() = 0;

      /*!
       * executes this statement
       * @return true if successful
       */
      virtual bool result() = 0;

      /*!
       * gets the last number of changes
       * @return the last number of changes or zero
       */
      virtual sql_changes last_number_of_changes() = 0;

      /*!
       * gets the last error
       * @return the last error or an empty string
       */
      virtual std::string last_error() = 0;

      /*!
       * gets the last insert id
       * @return the last insert id or zero
       */
      virtual sql_id last_insert_id() = 0;
    };
}  // namespace coda::db

#endif

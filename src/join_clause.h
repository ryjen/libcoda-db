/*!
 * @file join_clause.h
 * representation of a join clause in a sql query
 */
#ifndef CODA_DB_JOIN_CLAUSE_H
#define CODA_DB_JOIN_CLAUSE_H

#include <string>
#include <unordered_map>
#include "sql_generator.h"
#include "sql_types.h"
#include "where_clause.h"

namespace coda::db {
  namespace join {
    /*! types of sql joins */
    typedef enum { none, inner, left, right, natural, full, cross } type;
  }  // namespace join

  /*!
   * a utility class aimed at making join statements
   * ex. join("tablename").on("a", "b");
   */
  class join_clause : public sql_generator {
   public:
   private:
    std::string tableName_;
    join::type type_;
    where_clause on_;

    std::string generate_sql() const override;

    void set_modified();

   public:
    /*! default no-arg constructor */
    join_clause();

    /*!
     * @param tableName the required name of the table to join
     * @param joinType the type of sql query (default inner)
     */
    explicit join_clause(const std::string &tableName, join::type type = join::none);

    join_clause(const std::string &tableName, const std::string &alias, join::type type = join::none);

    join_clause(const join_clause &other) = default;

    join_clause(join_clause &&other) = default;

    join_clause &operator=(const join_clause &other) = default;

    join_clause &operator=(join_clause &&other) = default;

    ~join_clause() = default;

    /*!
     * tests if the join is empty
     * @return true if the sql is not valid
     */
    bool empty() const;

    /*!
     * resets the sql
     */
    void reset() override;

    /*!
     * sets the join type
     * @param value the join type
     */
    join_clause &type(join::type value);

    /*!
     * gets the type of join
     * @return the join type
     */
    join::type type() const;

    /*!
     * sets the table name
     * @param value the table name to set
     */
    join_clause &table(const std::string &value);

    join_clause &table(const std::string &value, const std::string &alias);

    /*!
     * gets the table for this join
     * @return the table name
     */
    std::string table() const;

    /*!
     * sets the sql to join on (example 'col1 = col2')
     * @param value the sql to join on
     * @return a where clause
     */
    where_clause &on(const std::string &value);

    /*!
     * sets the where clause to join on
     * @param value the where clause
     */
    join_clause &on(const where_clause &value);

    /*!
     * get the on portion of the join clause
     * @return the where clause
     */
    const where_clause &on() const;

    explicit operator std::string();

    explicit operator std::string() const;
  };

  /*!
   * stream operator for joins
   */
  std::ostream &operator<<(std::ostream &out, const join_clause &join);
}  // namespace coda::db

#endif

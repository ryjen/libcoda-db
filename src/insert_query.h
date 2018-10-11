#ifndef CODA_DB_INSERT_QUERY_H
#define CODA_DB_INSERT_QUERY_H

#include "modify_query.h"

namespace coda::db {
  /*!
   * a query to insert to a table
   */
  class insert_query : public modify_query {
   public:
    using modify_query::modify_query;

    /*!
     * @param db the database to modify
     * @param tableName the table to modify
     * @param columns the columns to modify
     */
    insert_query(const std::shared_ptr<coda::db::session> &session, const std::string &tableName);

    /*!
     * @param db the database to modify
     * @param columns the columns to modify
     */
    insert_query(const std::shared_ptr<coda::db::session> &session, const std::string &tableName,
                 const std::vector<std::string> &columns);

    /*!
     * @param schema the schema to modify
     * @param column the specific columns to modify in the schema
     */
    insert_query(const std::shared_ptr<schema> &schema, const std::vector<std::string> &columns);

    insert_query(const insert_query &other) = default;

    insert_query(insert_query &&other) noexcept;

    ~insert_query() override = default;

    insert_query &operator=(const insert_query &other);

    insert_query &operator=(insert_query &&other) noexcept;

    /*!
     * @return the id column of the last insert
     */
    sql_id last_insert_id() const;

    /*!
     * get the columns being modified
     * @return the list of columns
     */
    std::vector<std::string> columns() const;

    /*!
     * set the columns to modify
     * @param  value the list of column names
     * @return       a reference to this instance
     */
    insert_query &columns(const std::vector<std::string> &value);

    template<typename... List>
    insert_query &columns(const std::string &value, const List &... args) {
      column(value);
      columns(args...);
      return *this;
    }

    /*!
     * set the table to insert into
     * @see modify_query::table_name
     * @param  tableName the table name
     * @return           a reference to this instance
     */
    insert_query &into(const std::string &tableName);

    /*!
     * get the table name being inserted into
     * @see modify_query::table_name
     * @return the table name
     */
    std::string into() const;

    /*!
     * binds values to the query.  similar to bind_all but makes more sense in
     * a query context
     * @param value a value to bind
     * @param argv a variadic list of values to bind
     * @return a reference to this instance
     */
    template<typename T, typename... List>
    insert_query &values(const T &value, const List &... argv) {
      bind_list(1, value, argv...);
      return *this;
    }

    insert_query &values(const std::vector<sql_value> &value);

    insert_query &values(const std::unordered_map<std::string, sql_value> &value);

    insert_query &value(const std::string &name, const sql_value &value);

    insert_query &value(const sql_value &value);

    /*!
     * executes the insert query
     * @return the number of records inserted
     */
    sql_changes execute() override;

    /*!
     * tests if this query is valid
     * @return true if valid
     */
    bool is_valid() const noexcept override;

   private:
    insert_query &column(const std::string &value) {
      if (!value.empty()) {
        columns_.push_back(value);
        set_modified();
      }
      return *this;
    }

    std::string generate_sql() const override;

    sql_id lastId_;
    std::vector<std::string> columns_;
    std::string tableName_;
  };
}  // namespace coda::db

#endif

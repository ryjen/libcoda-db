/*!
 * @file modify_query.h
 * representation of sql queries that modify data
 * @copyright ryan jennings (coda.life), 2013
 */
#ifndef CODA_DB_MODIFY_QUERY_H
#define CODA_DB_MODIFY_QUERY_H

#include "query.h"

namespace coda::db {
  class session;

  /*!
   * a query to modify a table
   */
  class modify_query : public query {
   public:
    /*!
     * @param db the database in use
     */
    explicit modify_query(const std::shared_ptr<coda::db::session> &session);

    /*!
     * @param schema the schema to modify
     */
    explicit modify_query(const std::shared_ptr<schema> &schema);

    /* boilerplate */
    modify_query(const modify_query &other) = default;

    modify_query(modify_query &&other) noexcept = default;

    ~modify_query() override = default;

    modify_query &operator=(const modify_query &other) = default;

    modify_query &operator=(modify_query &&other) noexcept = default;

    /*!
     * executes this query using a replace statement
     * @return the last number of changes made by this query
     */
    virtual sql_changes execute();

    /*!
     * @return the last number of changes made by this query
     */
    sql_changes last_number_of_changes() const;

   protected:
    int flags_;
    sql_changes numChanges_;
  };
}  // namespace coda::db

#endif

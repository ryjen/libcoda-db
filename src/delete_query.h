#ifndef CODA_DB_DELETE_QUERY_H
#define CODA_DB_DELETE_QUERY_H

#include "modify_query.h"
#include "where_clause.h"

namespace coda {
  namespace db {
    /*!
     * a query to delete from a table
     */
    class delete_query : public modify_query, public whereable<delete_query> {
      public:
      delete_query(const std::shared_ptr<coda::db::session> &session);

      delete_query(const std::shared_ptr<coda::db::session> &session,
                   const std::string &tableName);

      delete_query(const std::shared_ptr<schema> &schema);

      /* boilerplate */
      delete_query(const delete_query &other);

      delete_query(delete_query &&other);

      virtual ~delete_query();

      delete_query &operator=(const delete_query &other);

      delete_query &operator=(delete_query &&other);

      /*!
       * set the table to insert into
       * @see modify_query::table_name
       * @param  tableName the table name
       * @return           a reference to this instance
       */
      delete_query &from(const std::string &tableName);

      /*!
       * get the table name being inserted into
       * @see modify_query::table_name
       * @return the table name
       */
      std::string from() const;

      /*!
       * gets the where builder for the query
       * @return a reference to the where builder
       */
      where_builder &where();

      /*!
       * sets the where clause using an operator
       * @param value the sql operator
       * @return a reference to the where builder
       */
      where_builder &where(const sql_operator &value);

      /*!
       * sets the where clause for the update query
       * @param value the where clause to set
       */
      delete_query &where(const where_clause &value);

#ifdef ENABLE_PARAMETER_MAPPING
      where_builder &where(const std::string &sql);

      /*!
       * adds a where clause to this query and binds parameters to it
       * @param value the sql where string
       * @param args the variadic list of bind values
       * @return a reference to this instance
       */
      template <typename... List>
      delete_query &where(const std::string &value, const List &... args) {
        where(value);
        bind_all(args...);
        return *this;
      }

      /*!
       * adds a where clause and binds parameters to it
       * @param value the where clause
       * @param args a variadic list of bind values
       * @return a reference to this instance
       */
      template <typename... List>
      delete_query &where(const where_clause &value, const List &... args) {
        where(value);
        bind_all(args...);
        return *this;
      }
#endif

      /*!
       * tests if this query is valid
       * @return true if valid
       */
      bool is_valid() const noexcept;

      private:
      std::string generate_sql() const;

      where_builder where_;
      std::string tableName_;
    };
  } // namespace db
} // namespace coda

#endif

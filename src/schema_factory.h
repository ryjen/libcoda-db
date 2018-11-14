/*!
 * @file schema_factory.h
 * a factory for caching schemas
 */
#ifndef CODA_DB_SCHEMA_FACTORY_H
#define CODA_DB_SCHEMA_FACTORY_H

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace coda {
  namespace db {
    class session;
    class schema;

    /*!
     *  Schema factory handles caching of table schemas.
     *  Caching the schemas reduces the amount of db reads when working
     *  with records.
     */
    class schema_factory {
      private:
      std::unordered_map<std::string, std::shared_ptr<schema>> schema_cache_;
      std::shared_ptr<schema> create(const std::shared_ptr<session> &session,
                                     const std::string &tableName);

      public:
      /*
       * default constructor
       */
      schema_factory();

      /* boilerplate */
      schema_factory(schema_factory &&other);
      schema_factory &operator=(schema_factory &&other);
      schema_factory(const schema_factory &other);
      virtual ~schema_factory();
      schema_factory &operator=(const schema_factory &other);
      std::shared_ptr<schema> get(const std::shared_ptr<session> &session,
                                  const std::string &tableName);

      /*!
       * clears a cached schema for the table name
       * @param tablename the table name to clear the schema for
       */
      void clear(const std::string &tablename);
    };
  } // namespace db
} // namespace coda
#endif

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

namespace coda::db {
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
    std::shared_ptr<schema> create(const std::shared_ptr<session> &session, const std::string &tableName);

   public:
    /*
     * default constructor
     */
    schema_factory() = default;

    /* boilerplate */
    schema_factory(schema_factory &&other) noexcept = default;
    schema_factory(const schema_factory &other) = default;
    ~schema_factory() = default;
    schema_factory &operator=(const schema_factory &other) = default;
    schema_factory &operator=(schema_factory &&other) noexcept = default;

    /**
     * gets the schema given a session and a table name
     * @param session  the database session
     * @param tableName the name of the table
     * @return the schema for the table name in the session
     */
    std::shared_ptr<schema> get(const std::shared_ptr<session> &session, const std::string &tableName);

    /*!
     * clears a cached schema for the table name
     * @param tableName the table name to clear the schema for
     */
    void clear(const std::string &tableName);
  };
}  // namespace coda::db
#endif

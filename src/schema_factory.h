#ifndef ARG3_DB_SCHEMA_FACTORY_H
#define ARG3_DB_SCHEMA_FACTORY_H

#include <unordered_map>
#include <functional>
#include <memory>
#include "schema.h"

namespace arg3
{
    namespace db
    {
        class sqldb;

        /*!
         *  Schema factory handles caching of table schemas.
         *  Caching the schemas reduces the amount of db reads when working
         *  with records.
         */
        class schema_factory
        {
           private:
            std::unordered_map<std::string, std::shared_ptr<schema>> schema_cache_;

            sqldb *db_;

            std::shared_ptr<schema> create(const std::string &tableName);

           public:
            schema_factory(sqldb *db);

            schema_factory(schema_factory &&other);

            schema_factory &operator=(schema_factory &&other);

            schema_factory(const schema_factory &other);

            schema_factory &operator=(const schema_factory &other);
            std::shared_ptr<schema> get(const std::string &tableName);

            void clear(const std::string &tablename);
        };
    }
}
#endif

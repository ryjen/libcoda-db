#ifndef ARG3_DB_SCHEMA_FACTORY_H_
#define ARG3_DB_SCHEMA_FACTORY_H_

#include <unordered_map>
#include <functional>
#include <memory>
#include "schema.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        class sqldb;

        class schema_factory
        {
        private:
            std::unordered_map<string, std::weak_ptr<schema>> schema_cache_;

            sqldb *db_;

            shared_ptr<schema> create(const string &tableName);

            schema_factory(const schema_factory &other);

            schema_factory &operator=(const schema_factory &other);

        public:
            schema_factory(sqldb *db);

            schema_factory(schema_factory &&other);

            schema_factory &operator=(schema_factory && other);

            std::shared_ptr<schema> get(const string &tableName);

            void unregister(schema *p);

            void clear(const string &tablename);
        };


    }
}
#endif

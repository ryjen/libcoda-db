#ifndef ARG3_DB_RECORD_SCHEMA_
#define ARG3_DB_RECORD_SCHEMA_

#include "schema.h"
#include <map>

namespace arg3
{
    namespace db
    {
        class record_schema : public schema
        {
        private:
            static map<string, std::shared_ptr<record_schema>> cache_;
        public:
            static constexpr const char *const ID_COLUMN_NAME = "id";

            record_schema();

            virtual ~record_schema();

            record_schema(const record_schema &other);

            record_schema(record_schema &&other);

            record_schema &operator=(const record_schema &other);

            record_schema &operator=(record_schema && other);

            static std::shared_ptr<record_schema> get(sqldb *db, const string &tableName);

            static void clear(const string &tableName);

            virtual void init(sqldb *db, const string &tableName);
        };
    }
}

#endif

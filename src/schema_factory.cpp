
#include "schema_factory.h"
#include "schema.h"
#include "sqldb.h"

namespace arg3
{
    namespace db
    {

        shared_ptr<schema> schema_factory::create(const string &tableName)
        {
            shared_ptr<schema> p = make_shared<schema>(db_, tableName);
            schema_cache_[tableName] = p;
            return p;
        }

        schema_factory &schema_factory::operator=(const schema_factory &other)
        {
            schema_cache_ = other.schema_cache_;
            db_ = other.db_;

            return *this;
        }


        schema_factory::schema_factory(const schema_factory &other) : schema_cache_(other.schema_cache_), db_(other.db_)
        {

        }

        schema_factory::schema_factory(sqldb *db) : db_(db)
        {
            assert(db != NULL);
        }

        schema_factory::schema_factory(schema_factory &&other) : schema_cache_(std::move(other.schema_cache_)), db_(other.db_)
        {}

        schema_factory &schema_factory::operator=(schema_factory && other)
        {
            schema_cache_ = std::move(other.schema_cache_);
            db_ = other.db_;

            other.db_ = NULL;

            return *this;
        }

        std::shared_ptr<schema> schema_factory::get(const string &tableName)
        {
            auto i = schema_cache_.find(tableName);
            if (i == schema_cache_.end())
            {
                return create(tableName);
            }
            else
            {
                return i->second;
            }
        }

        void schema_factory::clear(const string &tablename)
        {
            schema_cache_.erase(tablename);
        }
    }
}
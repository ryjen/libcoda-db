
#include "schema_factory.h"
#include "schema.h"
#include "sqldb.h"

namespace arg3
{
    namespace db
    {

        shared_ptr<schema> schema_factory::create(const string &tableName)
        {
            shared_ptr<schema> p(new schema(tableName), std::bind(&schema_factory::unregister, this, std::placeholders::_1));
            schema_cache_[tableName] = p;
            p->init(db_);
            return p;
        }

        schema_factory &schema_factory::operator=(const schema_factory &other)
        {
            if (this != &other)
            {
                schema_cache_ = other.schema_cache_;
                db_ = other.db_;
            }
            return *this;
        }


        schema_factory::schema_factory(const schema_factory &other) : schema_cache_(other.schema_cache_), db_(other.db_)
        {

        }

        schema_factory::schema_factory(sqldb *db) : db_(db)
        {
            assert(db != NULL);
        }

        schema_factory::schema_factory(schema_factory &&other) : schema_cache_(std::move(other.schema_cache_)), db_(std::move(other.db_))
        {}

        schema_factory &schema_factory::operator=(schema_factory && other)
        {
            if (this != &other)
            {
                schema_cache_ = std::move(other.schema_cache_);
                db_ = std::move(other.db_);

                other.db_ = NULL;
            }
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
                shared_ptr<schema> p = i->second.lock();
                if (p == nullptr)
                {
                    p = create(tableName);
                }

                return p;
            }
        }

        void schema_factory::unregister(schema *p)
        {
            if (p == NULL) return;

            auto id = p->table_name();
            schema_cache_.erase(id);
            delete p;
        }

        void schema_factory::clear(const string &tablename)
        {
            schema_cache_.erase(tablename);
        }
    }
}
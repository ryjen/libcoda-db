
#include "schema_factory.h"
#include "exception.h"
#include "schema.h"

using namespace std;

namespace coda
{
    namespace db
    {
        shared_ptr<schema> schema_factory::create(const std::shared_ptr<session> &session, const string &tableName)
        {
            shared_ptr<schema> p = nullptr;

            if (session == nullptr) {
                throw database_exception("invalid session for schema create");
            }
            p = make_shared<schema>(session, tableName);
            schema_cache_[tableName] = p;
            return p;
        }

        schema_factory &schema_factory::operator=(const schema_factory &other)
        {
            schema_cache_ = other.schema_cache_;
            return *this;
        }


        schema_factory::schema_factory(const schema_factory &other) : schema_cache_(other.schema_cache_)
        {
        }

        schema_factory::schema_factory()
        {
        }

        schema_factory::~schema_factory()
        {
        }

        schema_factory::schema_factory(schema_factory &&other) : schema_cache_(std::move(other.schema_cache_))
        {
        }

        schema_factory &schema_factory::operator=(schema_factory &&other)
        {
            schema_cache_ = std::move(other.schema_cache_);

            return *this;
        }

        std::shared_ptr<schema> schema_factory::get(const std::shared_ptr<session> &session, const string &tableName)
        {
            auto i = schema_cache_.find(tableName);

            if (i == schema_cache_.end()) {
                return create(session, tableName);
            } else {
                return i->second;
            }
        }

        void schema_factory::clear(const string &tablename)
        {
            schema_cache_.erase(tablename);
        }
    }
}

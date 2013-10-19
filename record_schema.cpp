#include "record_schema.h"
#include "resultset.h"

namespace arg3
{
    namespace db
    {
        map<string, shared_ptr<record_schema>> record_schema::cache_;

        record_schema::record_schema() : schema() {}

        record_schema::~record_schema() {}

        record_schema::record_schema(const record_schema &other) : schema(other)
        {}

        record_schema::record_schema(record_schema &&other) : schema(other)
        {}

        record_schema &record_schema::operator=(const record_schema &other)
        {
            if (this != &other)
            {
                schema::operator=(other);
            }
            return *this;
        }

        record_schema &record_schema::operator=(record_schema && other)
        {
            if (this != &other)
            {
                schema::operator=(other);
            }
            return *this;
        }
        std::shared_ptr<record_schema> record_schema::get(sqldb *db, const string &tableName)
        {
            if (cache_.count(tableName))
            {
                return cache_[tableName];
            }
            else
            {
                shared_ptr<record_schema> s = make_shared<record_schema>();

                s->init(db, tableName);

                cache_[tableName] = s;

                return s;
            }
        }

        void record_schema::clear(const string &tableName)
        {
            cache_.erase(tableName);
        }

        void record_schema::init(sqldb *db, const string &tableName)
        {
            auto rs = db->execute( "pragma table_info(" + tableName + ")" );

            bool found = false;
            for (auto row : rs)
            {
                if (row["name"].to_string() == "id")
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                db->execute("alter table " + tableName + " add column id varchar(64)");
                db->execute("create unique index if not exists record_id_index on " + tableName + " (id)");
            }
            schema::init(db, tableName);
        }
    }
}

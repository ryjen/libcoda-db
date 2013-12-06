#include "schema.h"
#include <cassert>
#include  "sqldb.h"
#include "resultset.h"

namespace arg3
{

    namespace db
    {
        ostream &operator<<(ostream &os, const column_definition &def)
        {
            os << def.name;
            return os;
        }

        schema::schema(const string &tablename) : tableName_(tablename) {}

        schema::~schema() {}

        schema::schema(const schema &other) : db_(other.db_), tableName_(other.tableName_), columns_(other.columns_)
        {}

        schema::schema(schema &&other) : db_(std::move(other.db_)), tableName_(std::move(other.tableName_)), columns_(std::move(other.columns_))
        {}

        schema &schema::operator=(const schema &other)
        {
            if (this != &other)
            {
                columns_ = other.columns_;
                db_ = other.db_;
                tableName_ = other.tableName_;
            }
            return *this;
        }

        schema &schema::operator=(schema && other)
        {
            if (this != &other)
            {
                columns_ = std::move(other.columns_);
                db_ = std::move(other.db_);
                tableName_ = std::move(other.tableName_);
            }
            return *this;
        }

        bool schema::is_valid() const
        {
            return columns_.size() > 0;
        }

        void schema::init(sqldb *db)
        {
            assert(db != NULL);

            assert(db->is_open());

            assert(!tableName_.empty());

            db_ = db;

            // get table information
            auto rs = db->execute( "pragma table_info(" + tableName_ + ")" );

            for (auto & row : rs)
            {
                column_definition def;

                // column name
                def.name = row["name"].to_string();

                // primary key check
                def.pk = row["pk"].to_bool();

                // find type
                string type = row["type"].to_string();

                if (type.find("integer") != string::npos)
                {
                    def.type = SQLITE_INTEGER;
                }
                else if (type.find("real") != string::npos)
                {
                    def.type = SQLITE_FLOAT;
                }
                else if (type.find("blob") != string::npos)
                {
                    def.type = SQLITE_BLOB;
                }
                else
                {
                    def.type = SQLITE_TEXT;
                }

                columns_.push_back(def);
            }
        }

        vector<column_definition> schema::columns() const
        {
            return columns_;
        }

        vector<string> schema::column_names() const
        {
            vector<string> names;

            for (auto & c : columns_)
            {
                names.push_back(c.name);
            }
            return names;
        }

        vector<string> schema::primary_keys() const
        {
            vector<string> names;

            for (auto & c : columns_)
            {
                if (c.pk)
                    names.push_back(c.name);
            }

            return names;
        }

        string schema::table_name() const
        {
            return tableName_;
        }

        sqldb *schema::db() const
        {
            return db_;
        }

        column_definition schema::operator[](size_t index) const
        {
            return columns_[index];
        }
    };

}

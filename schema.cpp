#include "schema.h"
#include "../format/format.h"
#include <cassert>
#include "resultset.h"

namespace arg3
{

    namespace db
    {
        string column_definition::name() const
        {
            return name_;
        }
        bool column_definition::pk() const
        {
            return pk_;
        }

        int column_definition::type() const
        {
            return type_;
        }

        ostream &operator<<(ostream &os, const column_definition &def)
        {
            os << def.name();
            return os;
        }

        schema::schema() {}

        schema::schema(sqldb db, const string &tablename)
        {
            init(db, tablename);
        }

        bool schema::is_valid() const
        {
            return columns_.size() > 0;
        }

        void schema::init(sqldb db, const string &tablename)
        {
            assert(db.is_open());

            // get table information
            auto rs = db.execute( format("pragma table_info({0})", tablename));

for(auto &row : rs)
            {
                column_definition def;

                // column name
                def.name_ = row["name"].to_string();

                // primary key check
                def.pk_ = row["pk"].to_bool();

                // find type
                string type = row["type"].to_string();

                if(type.find("integer") != string::npos)
                {
                    def.type_ = SQLITE_INTEGER;
                }
                else if(type.find("real") != string::npos)
                {
                    def.type_ = SQLITE_FLOAT;
                }
                else if(type.find("blob") != string::npos)
                {
                    def.type_ = SQLITE_BLOB;
                }
                else
                {
                    def.type_ = SQLITE_TEXT;
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

for(auto &c : columns_)
            {
                names.push_back(c.name());
            }
            return names;
        }

        vector<string> schema::primary_keys() const
        {
            vector<string> names;

for(auto &c : columns_)
            {
                if(c.pk())
                    names.push_back(c.name());
            }

            return names;
        }

        column_definition schema::operator[](size_t index) const
        {
            return columns_[index];
        }
    };

}

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
            return m_name;
        }
        bool column_definition::pk() const
        {
            return m_pk;
        }

        int column_definition::type() const
        {
            return m_type;
        }


        ostream &operator<<(ostream &os, const column_definition &def) {
        	os << def.name();
        	return os;
        }

        schema::schema() {}

        schema::schema(sqldb db, const string &tablename)
        {
            init(db, tablename);
        }

        bool schema::is_valid() const {
        	return m_columns.size() > 0;
        }

        void schema::init(sqldb db, const string &tablename) 
        {
            assert(db.is_open());

            auto rs = db.execute( format("pragma table_info({0})", tablename));

            for(auto &row : rs)
            {
	    		column_definition def;

	            def.m_name = row[1].to_string();
	            def.m_pk = row[5].to_int() == 1;

	            string type = row[2].to_string();

	            if(type.find("integer") != string::npos) {
	            	def.m_type = SQLITE_INTEGER;
	            }
	            else if(type.find("real") != string::npos) {
	            	def.m_type = SQLITE_FLOAT;
	            }
	            else if(type.find("blob") != string::npos) {
	            	def.m_type = SQLITE_BLOB;
	            }
	            else {
	            	def.m_type = SQLITE_TEXT;
	            }

	            m_columns.push_back(def);
            }
        }

        vector<column_definition> schema::columns() const
        {
            return m_columns;
        }
    };

}

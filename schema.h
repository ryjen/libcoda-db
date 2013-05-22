#ifndef _ARG3_DB_SCHEMA_H_
#define _ARG3_DB_SCHEMA_H_

#include <string>
#include <vector>
#include "sqldb.h"

using namespace std;

namespace arg3
{

    namespace db
    {
        // definition of a column
        class column_definition
        {
            friend class schema;
        private:
            string name_;
            bool pk_;
            int type_;
        public:
            string name() const;

            bool pk() const;

            int type() const;
        };

        ostream &operator<<(ostream &os, const column_definition &def);

        // definition of a table
        class schema
        {
        private:
            vector<column_definition> columns_;

        public:
            schema();

            virtual ~schema();

            schema(sqldb *db, const string &tablename);

            schema(const schema &other);

            schema(schema &&other);

            schema &operator=(const schema &other);
            schema &operator=(schema &&other);

            void init(sqldb *db, const string &tablename);

            vector<column_definition> columns() const;

            vector<string> column_names() const;

            vector<string> primary_keys() const;

            column_definition operator[](size_t index) const;

            bool is_valid() const;
        };
    }

}

#endif

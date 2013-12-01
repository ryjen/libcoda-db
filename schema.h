#ifndef _ARG3_DB_SCHEMA_H_
#define _ARG3_DB_SCHEMA_H_

#include <string>
#include <vector>

using namespace std;

namespace arg3
{

    namespace db
    {
        class sqldb;

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
            sqldb *db_;
            string tableName_;
            vector<column_definition> columns_;
        public:
            schema(const string &tablename);

            virtual ~schema();

            schema(const schema &other);

            schema(schema &&other);

            schema &operator=(const schema &other);
            schema &operator=(schema && other);

            virtual void init(sqldb *db);

            vector<column_definition> columns() const;

            vector<string> column_names() const;

            vector<string> primary_keys() const;

            string table_name() const;

            column_definition operator[](size_t index) const;

            bool is_valid() const;

            sqldb *db() const;
        };
    }
}

#endif

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

        /*!
         * Definition of a column in a schema
         */
        struct column_definition
        {
            string name;
            bool pk;
            int type;
        };

        ostream &operator<<(ostream &os, const column_definition &def);

        /*!
         * Schema is a definition of a table in a database
         * Allows for quick access to column names and other information
         */
        class schema
        {
        private:
            sqldb *db_;
            string tableName_;
            vector<column_definition> columns_;
        public:
            schema(sqldb *db, const string &tablename);

            virtual ~schema();

            schema(const schema &other);

            schema(schema &&other);

            schema &operator=(const schema &other);
            schema &operator=(schema && other);

            virtual void init();

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

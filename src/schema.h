#ifndef ARG3_DB_SCHEMA_H
#define ARG3_DB_SCHEMA_H

#include <string>
#include <vector>

namespace arg3
{
    namespace db
    {
        class sqldb;

        /*!
         * Definition of a column in a schema
         */
        struct column_definition {
            std::string name;
            bool pk;
            std::string type;
        };

        std::ostream &operator<<(std::ostream &os, const column_definition &def);

        /*!
         * Schema is a definition of a table in a database
         * Allows for quick access to column names and other information
         */
        class schema
        {
           private:
            sqldb *db_;
            std::string tableName_;
            std::vector<column_definition> columns_;

           public:
            schema(sqldb *db, const std::string &tablename);

            virtual ~schema();

            schema(const schema &other);

            schema(schema &&other);

            schema &operator=(const schema &other);
            schema &operator=(schema &&other);

            virtual void init();

            std::vector<column_definition> columns() const;

            std::vector<std::string> column_names() const;

            std::vector<std::string> primary_keys() const;

            std::string table_name() const;

            column_definition operator[](size_t index) const;

            bool is_valid() const;

            sqldb *db() const;
        };
    }
}

#endif

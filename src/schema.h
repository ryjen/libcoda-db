/*!
 * @file schema.h
 * table definition in a database
 */
#ifndef ARG3_DB_SCHEMA_H
#define ARG3_DB_SCHEMA_H

#include <string>
#include <vector>
#include <memory>

namespace arg3
{
    namespace db
    {
        class session;

        /*!
         * Definition of a column in a schema
         */
        struct column_definition {
            std::string name;
            bool pk;
            bool autoincrement;
            std::string type;
        };

        /*!
         * output stream append operator for a column definition
         */
        std::ostream &operator<<(std::ostream &os, const column_definition &def);

        /*!
         * Schema is a definition of a table in a database
         * Allows for quick access to column names and other information
         */
        class schema
        {
           private:
            std::shared_ptr<session> session_;
            std::string tableName_;
            std::vector<column_definition> columns_;

           public:
            /*!
             * @param db the database in use
             * @param tablename the tablename to query
             */
            schema(const std::shared_ptr<session> &session, const std::string &tablename);

            /* boilerplate */
            virtual ~schema();
            schema(const schema &other);
            schema(schema &&other);
            schema &operator=(const schema &other);
            schema &operator=(schema &&other);

            /*!
             * initializes this schema
             */
            virtual void init();

            /*!
             * @return the column definitions for this schema
             */
            std::vector<column_definition> columns() const;

            /*!
             * @return the column names for this schema
             */
            std::vector<std::string> column_names() const;

            /*!
             * @return the primary keys for this schema
             */
            std::vector<std::string> primary_keys() const;

            /*!
             * gets the only auto incrementing primary key in a table
             * @return the key name
             */
            std::string primary_key() const;

            /*!
             * gets the table name for this schema
             * @return the table name string
             */
            std::string table_name() const;

            /*!
             * gets a column definition by index
             * @param  index the index of the column definition
             * @return       a column definition object
             */
            column_definition operator[](size_t index) const;

            /*!
             * gets the number of columns in this schema
             * @return the number of columns
             */
            size_t size() const;

            /*!
             * tests if this schema is valid
             * @return true if valid
             */
            bool is_valid() const;

            /*!
             * gets the database for this schema
             * @return the database object
             */
            std::shared_ptr<session> session() const;
        };
    }
}

#endif

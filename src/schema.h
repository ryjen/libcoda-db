/*!
 * @file schema.h
 * table definition in a database
 */
#ifndef CODA_DB_SCHEMA_H
#define CODA_DB_SCHEMA_H

#include <memory>
#include <string>
#include <vector>

namespace coda {
    namespace db {
        class session;

        class sql_value;

        /*!
         * Definition of a column in a schema
         */
        struct column_definition {
            std::string name;
            bool pk;
            bool autoincrement;
            std::string type;
            std::string default_value;
        };

        /*!
         * output stream append operator for a column definition
         */
        std::ostream &operator<<(std::ostream &os, const column_definition &def);

        /*!
         * Schema is a definition of a table in a database
         * Allows for quick access to column names and other information
         */
        class schema {
        public:
            typedef session session_type;

        private:
            std::shared_ptr<session_type> session_;
            std::string tableName_;
            std::vector<column_definition> columns_;

        public:
            /*!
             * @param db the database in use
             * @param tablename the tablename to query
             */
            schema(const std::shared_ptr<session_type> &sess, const std::string &tablename);

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
            std::vector<column_definition> columns() const noexcept;

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
             * gets the default value for a column
             * @param name the name of the column
             */
            sql_value default_value(const std::string &name) const;

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
            size_t size() const noexcept;

            /*!
             * tests if this schema is valid
             * @return true if valid
             */
            bool is_valid() const noexcept;

            /*!
             * gets the database for this schema
             * @return the database object
             */
            std::shared_ptr<schema::session_type> get_session() const;
        };
    }
}

#endif

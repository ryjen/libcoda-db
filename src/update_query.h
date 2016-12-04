#ifndef RJ_DB_UPDATE_QUERY_H
#define RJ_DB_UPDATE_QUERY_H

#include "modify_query.h"
#include "where_clause.h"

namespace rj
{
    namespace db
    {
        /*!
         * a query to update a table
         */
        class update_query : public modify_query, public whereable<update_query>
        {
           public:
            /*!
             * @param db the database in use
             */
            update_query(const std::shared_ptr<rj::db::session> &session);

            /*!
             * @param schema the schema to modify
             */
            update_query(const std::shared_ptr<schema> &schema);

            /*!
             * @param db the database to modify
             * @param tableName the table to modify
             * @param columns the columns to modify
             */
            update_query(const std::shared_ptr<rj::db::session> &session, const std::string &tableName);

            /*!
             * @param db the database to modify
             * @param columns the columns to modify
             */
            update_query(const std::shared_ptr<rj::db::session> &session, const std::string &tableName, const std::vector<std::string> &columns);

            /*!
             * @param schema the schema to modify
             * @param column the specific columns to modify in the schema
             */
            update_query(const std::shared_ptr<schema> &schema, const std::vector<std::string> &columns);

            /* boilerplate */
            update_query(const update_query &other);
            update_query(update_query &&other);
            virtual ~update_query();
            update_query &operator=(const update_query &other);
            update_query &operator=(update_query &&other);

            /*!
             * get the columns being modified
             * @return the list of columns
             */
            std::vector<std::string> columns() const;

            /*!
             * set the columns to modify
             * @param  value the list of column names
             * @return       a reference to this instance
             */
            update_query &columns(const std::vector<std::string> &value);

            template <typename... List>
            update_query &columns(const std::string &value, const List &... args)
            {
                column(value);
                columns(args...);
                return *this;
            }

            /*!
             * set the table to insert into
             * @see modify_query::table_name
             * @param  tableName the table name
             * @return           a reference to this instance
             */
            update_query &table(const std::string &tableName);

            /*!
             * get the table name being inserted into
             * @see modify_query::table_name
             * @return the table name
             */
            std::string table() const;

            /*!
             * gets the where builder for the query
             * @return a reference to the where builder
             */
            where_builder &where();

            /*!
             * sets the where clause for the update query
             * @param value the where clause to set
             */
            update_query &where(const where_clause &value);

            /*!
             * gets the where clause
             * @return the where clause
             */
            where_builder &where(const sql_operator &value);

#ifdef ENABLE_PARAMETER_MAPPING
            where_builder &where(const std::string &value);

            /*!
            * adds a where clause to this query and binds parameters to it
            * @param value the sql where string
            * @param args the variadic list of bind values
            * @return a reference to this instance
            */
            template <typename... List>
            update_query &where(const std::string &value, const List &... args)
            {
                where(value);
                bind_all(args...);
                return *this;
            }

            /*!
             * adds a where clause and binds parameters to it
             * @param value the where clause
             * @param args a variadic list of bind values
             * @return a reference to this instance
             */
            template <typename... List>
            update_query &where(const where_clause &value, const List &... args)
            {
                where(value);
                bind_all(args...);
                return *this;
            }
#endif

            /*!
             * a rename of the bind_all method so it makes sense to the query language
             * @param value a value to bind
             * @param argv the variadic list of values to bind
             */
            template <typename T, typename... List>
            update_query &values(const T &value, const List &... argv)
            {
                bind_list(1, value, argv...);
                return *this;
            }

            update_query &values(const std::vector<sql_value> &value);

            update_query &values(const std::unordered_map<std::string, sql_value> &value);

            update_query &value(const std::string &name, const sql_value &value);

            update_query &value(const sql_value &value);

            /*!
             * @return the string/sql representation of this query
             */
            std::string to_string() const;

            /*!
             * tests if this query is valid
             * @return true if valid
             */
            bool is_valid() const;

           private:
            update_query &column(const std::string &value)
            {
                columns_.push_back(value);
                return *this;
            }

            where_builder where_;
            std::vector<std::string> columns_;
            std::string tableName_;
        };
    }
}

#endif

/*!
 * @file join_clause.h
 * representation of a join clause in a sql query
 */
#ifndef ARG3_DB_JOIN_CLAUSE_H
#define ARG3_DB_JOIN_CLAUSE_H

#include <unordered_map>
#include <string>
#include "where_clause.h"

namespace arg3
{
    namespace db
    {
        // define join types in a namespace
        namespace join
        {
            /*! types of sql joins */
            typedef enum { inner, left, right, outer } type;
        }

        /*!
         * a utility class aimed at making join statements
         * ex. join("tablename").on("a", "b");
         */
        class join_clause
        {
           private:
            std::string tableName_;
            join::type type_;
            where_clause on_;

           public:
            /*! default no-arg constructor */
            join_clause();

            /*! explit constructor
             * @param tableName the required name of the table to join
             * @param joinType the type of sql query (default inner)
             */
            explicit join_clause(const std::string &tableName, join::type joinType = join::inner);

            /*! boilerplate rule of 3 + move */
            join_clause(const join_clause &other);
            join_clause(join_clause &&other);
            join_clause &operator=(const join_clause &other);
            join_clause &operator=(join_clause &&other);
            virtual ~join_clause();

            /*!
             * @return a sql string representation of this join clause
             */
            std::string to_string() const;

            /*!
             * tests if the join is empty
             * @return true if the sql is not valid
             */
            bool empty() const;

            /*!
             * resets the sql
             */
            void reset();

            /*!
             * sets the join type
             * @param value the join type
             */
            join_clause &set_type(join::type value);

            /*!
             * sets the table name
             * @param value the table name to set
             */
            join_clause &set_table_name(const std::string &value);

            /*!
             * sets the sql to join on (example 'col1 = col2')
             * @param value the sql to join on
             * @return a where clause
             */
            where_clause &on(const std::string &value);

            /*!
             * sets the where clause to join on
             * @param value the where clause
             */
            join_clause &on(const where_clause &value);

            /*!
             * the explicit cast operator to sql string representation
             */
            explicit operator std::string();
        };

        /*!
         * stream operator for joins
         */
        std::ostream &operator<<(std::ostream &out, const join_clause &join);

        /*!
         * suffix operator
         * example: "tableName"_join.on("col1 = col2")
         */
        join_clause operator"" _join(const char *cstr, size_t len);
    }
}

#endif

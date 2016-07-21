/*!
 * @file join_clause.h
 * representation of a join clause in a sql query
 */
#ifndef RJ_DB_JOIN_CLAUSE_H
#define RJ_DB_JOIN_CLAUSE_H

#include <string>
#include <unordered_map>
#include "where_clause.h"

namespace rj
{
    namespace db
    {
        namespace join
        {
            /*! types of sql joins */
            typedef enum { none, inner, left, right, natural, full, cross } type;
        }

        /*!
         * a utility class aimed at making join statements
         * ex. join("tablename").on("a", "b");
         */
        class join_clause
        {
           public:
           private:
            std::string tableName_;
            join::type type_;
            where_clause on_;

           public:
            /*! default no-arg constructor */
            join_clause();

            /*!
             * @param tableName the required name of the table to join
             * @param joinType the type of sql query (default inner)
             */
            explicit join_clause(const std::string &tableName, join::type type = join::none);

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
            join_clause &type(join::type value);

            /*!
             * gets the type of join
             * @return the join type
             */
            join::type type() const;

            /*!
             * sets the table name
             * @param value the table name to set
             */
            join_clause &table(const std::string &value);

            /*!
             * gets the table for this join
             * @return the table name
             */
            std::string table() const;

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
             * get the on portion of the join clause
             * @return the where clause
             */
            const where_clause &on() const;

            /*!
             * the explicit cast operator to sql string representation
             */
            explicit operator std::string();
        };

        /*!
         * stream operator for joins
         */
        std::ostream &operator<<(std::ostream &out, const join_clause &join);
    }
}

#endif

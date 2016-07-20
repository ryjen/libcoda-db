/*!
 * @file where_clause.h
 */
#ifndef ARG3_DB_WHERE_CLAUSE_H
#define ARG3_DB_WHERE_CLAUSE_H

#include <string>
#include <vector>

#include "sqldb.h"

namespace arg3
{
    namespace db
    {
        /*!
         * a utility class aimed at making logic where statements
         * ex. where("a = b") || "c == d" && "e == f";
         */
        class where_clause
        {
           private:
            std::string value_;
            std::vector<where_clause> and_;
            std::vector<where_clause> or_;

           public:
            /*!
             * Default constructor
             */
            where_clause();
            /*!
             * construct a where clause with sql
             * @param  value the sql string
             */
            explicit where_clause(const std::string &value);

            /* boilerplate */
            where_clause(const where_clause &other);
            where_clause(where_clause &&other);
            where_clause &operator=(const where_clause &other);
            where_clause &operator=(where_clause &&other);
            virtual ~where_clause();

            /*!
             * the sql for this where clause
             * @return a sql string
             */
            virtual std::string to_string() const;

            /*!
             * explicit cast operator to sql string
             * @return the sql string
             */
            explicit operator std::string();

            /*!
             * Appends and AND part to this where clause
             * @param value   the sql to append
             */
            where_clause &operator&&(const std::string &value);
            /*!
             * Appends and AND part to this where clause
             * @param value   the sql to append
             */
            where_clause &operator&&(const where_clause &value);

            /*!
             * Appends and OR part to this where clause
             * @param value   the sql to append
             */
            where_clause &operator||(const where_clause &value);

            /*!
             * Appends and OR part to this where clause
             * @param value   the sql to append
             */
            where_clause &operator||(const std::string &value);

            /*!
             * Tests this where clause has sql
             * @return true if there is no sql in this clause
             */
            bool empty() const;

            /*!
             * resets this where clause
             */
            void reset();
        };

        /*!
         * output stream append operator
         * @param out   the output stream
         * @param where the where clause to append
         * @return the output stream
         */
        std::ostream &operator<<(std::ostream &out, const where_clause &where);

        /*!
         * simplify the type name
         */
        typedef where_clause where;
    }
}

#endif

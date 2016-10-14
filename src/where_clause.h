/*!
 * @file where_clause.h
 */
#ifndef RJ_DB_WHERE_CLAUSE_H
#define RJ_DB_WHERE_CLAUSE_H

#include <string>
#include <vector>

#include "bindable.h"
#include "sqldb.h"

namespace rj
{
    namespace db
    {
        namespace op
        {
            constexpr static const char *const EQ = "=";
            constexpr static const char *const NEQ = "!=";
            constexpr static const char *const LIKE = "LIKE";
            constexpr static const char *const IN = "IN";
            constexpr static const char *const BETWEEN = "BETWEEN";
        }

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
            void reset(const std::string &value);
            void reset(const where_clause &value);
        };

        class where_builder : public where_clause, public bindable
        {
           private:
            bindable *binder_;
            std::shared_ptr<session_impl> session_;

           public:
            where_builder(const std::shared_ptr<session_impl> &session, bindable *bindable);
            where_builder(const where_builder &other);
            where_builder(where_builder &&other);
            where_builder &operator=(const where_builder &other);
            where_builder &operator=(where_builder &&other);
            virtual ~where_builder();

            where_builder &equals(const std::string &column, const sql_value &value);
            where_builder &and_equals(const std::string &column, const sql_value &value);
            where_builder &or_equals(const std::string &column, const sql_value &value);

            where_builder &nequals(const std::string &column, const sql_value &value);
            where_builder &and_nequals(const std::string &column, const sql_value &value);
            where_builder &or_nequals(const std::string &column, const sql_value &value);

            where_builder &like(const std::string &column, const std::string &pattern);
            where_builder &and_like(const std::string &column, const std::string &pattern);
            where_builder &or_like(const std::string &column, const std::string &pattern);

            where_builder &in(const std::string &column, const std::vector<sql_value> &values);
            where_builder &and_in(const std::string &column, const std::vector<sql_value> &values);
            where_builder &or_in(const std::string &column, const std::vector<sql_value> &values);

            where_builder &between(const std::string &column, const sql_value &value1, const sql_value &value2);
            where_builder &and_between(const std::string &column, const sql_value &value1, const sql_value &value2);
            where_builder &or_between(const std::string &column, const sql_value &value1, const sql_value &value2);

            where_builder &bind(size_t index, const sql_value &value);
            where_builder &bind(const std::string &name, const sql_value &value);

            size_t num_of_bindings() const;


            /*!
             * Appends and AND part to this where clause
             * @param value   the sql to append
             */
            where_builder &operator&&(const std::string &value);
            /*!
             * Appends and AND part to this where clause
             * @param value   the sql to append
             */
            where_builder &operator&&(const where_clause &value);

            /*!
             * Appends and OR part to this where clause
             * @param value   the sql to append
             */
            where_builder &operator||(const where_clause &value);

            /*!
             * Appends and OR part to this where clause
             * @param value   the sql to append
             */
            where_builder &operator||(const std::string &value);
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

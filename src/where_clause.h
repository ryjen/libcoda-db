/*!
 * @file where_clause.h
 */
#ifndef CODA_DB_WHERE_CLAUSE_H
#define CODA_DB_WHERE_CLAUSE_H

#include <memory>
#include <string>
#include <vector>
#include "bindable.h"
#include "sql_generator.h"
#include "sql_value.h"

namespace coda
{
    namespace db
    {
        class session_impl;
        class sql_operator;

        typedef struct {
            std::string name;
            sql_value value;
        } named_param;

        named_param bind(const std::string &name, const sql_value &value);

        namespace op
        {
            typedef enum { EQ, LIKE, IN, BETWEEN, ISNULL, GREATER, LESSER, EQ_GREATER, EQ_LESSER } type;

            constexpr static const char *const type_values[] = {"=", "LIKE", "IN", "BETWEEN", "IS NULL",
                                                                ">", "<",    ">=", "<="};

            constexpr static const char *const not_type_values[] = {
                "!=", "NOT LIKE", "NOT IN", "NOT BETWEEN", "IS NOT NULL", "!>", "!<", "<", ">"};

            sql_operator equals(const sql_value &lvalue, const sql_value &rvalue);
            sql_operator greater(const sql_value &lvalue, const sql_value &rvalue);
            sql_operator lesser(const sql_value &lvalue, const sql_value &rvalue);
            sql_operator equals_greater(const sql_value &lvalue, const sql_value &rvalue);
            sql_operator equals_lesser(const sql_value &lvalue, const sql_value &rvalue);
            sql_operator like(const sql_value &lvalue, const std::string &rvalue);
            sql_operator startswith(const sql_value &lvalue, const std::string &rvalue);
            sql_operator endswith(const sql_value &lvalue, const std::string &rvalue);
            sql_operator contains(const sql_value &lvalue, const std::string &rvalue);
            sql_operator in(const sql_value &lvalue, const std::vector<sql_value> &rvalue);
            sql_operator between(const sql_value &lvalue, const sql_value &rvalue1, const sql_value &rvalue2);
            sql_operator is_null(const sql_value &lvalue);
        }


        class sql_operator
        {
            friend class where_builder;
            friend sql_operator op::equals(const sql_value &lvalue, const sql_value &rvalue);
            friend sql_operator op::greater(const sql_value &lvalue, const sql_value &rvalue);
            friend sql_operator op::lesser(const sql_value &lvalue, const sql_value &rvalue);
            friend sql_operator op::equals_greater(const sql_value &lvalue, const sql_value &rvalue);
            friend sql_operator op::equals_lesser(const sql_value &lvalue, const sql_value &rvalue);
            friend sql_operator op::like(const sql_value &lvalue, const std::string &rvalue);
            friend sql_operator op::startswith(const sql_value &lvalue, const std::string &rvalue);
            friend sql_operator op::endswith(const sql_value &lvalue, const std::string &rvalue);
            friend sql_operator op::contains(const sql_value &lvalue, const std::string &rvalue);
            friend sql_operator op::in(const sql_value &lvalue, const std::vector<sql_value> &rvalue);
            friend sql_operator op::between(const sql_value &lvalue, const sql_value &rvalue1,
                                            const sql_value &rvalue2);
            friend sql_operator op::is_null(const sql_value &lvalue);

           protected:
            void copy(const sql_operator &other);
            void move(sql_operator &&other);
            bool not_;
            sql_value lvalue_;
            union {
                sql_value rvalue_;
                std::vector<sql_value> rvalues_;
                std::pair<sql_value, sql_value> rrange_;
            };
            op::type type_;
            sql_operator();

           public:
            /* rule-of-5 */
            sql_operator(const sql_operator &other);
            sql_operator(sql_operator &&other);
            sql_operator &operator=(const sql_operator &other);
            sql_operator &operator=(sql_operator &&other);
            virtual ~sql_operator();

            sql_operator &operator!();
            op::type type() const;

            sql_value lvalue() const;

            bool is_named() const;

            template <typename T>
            void rvalue(const T &visitor)
            {
                switch (type_) {
                    case op::EQ:
                    case op::LIKE:
                    case op::ISNULL:
                    case op::GREATER:
                    case op::LESSER:
                    case op::EQ_GREATER:
                    case op::EQ_LESSER:
                        visitor(rvalue_);
                        break;
                    case op::IN:
                        visitor(rvalues_);
                        break;
                    case op::BETWEEN:
                        visitor(rrange_);
                        break;
                }
            }
        };

        class sql_operator_builder : public sql_operator
        {
           public:
            sql_operator_builder(const sql_value &lvalue);

            using sql_operator::sql_operator;
            using sql_operator::operator=;

            sql_operator_builder(const sql_operator_builder &other);
            sql_operator_builder(sql_operator_builder &&other);
            sql_operator_builder &operator=(const sql_operator_builder &other);
            sql_operator_builder &operator=(sql_operator_builder &&other);
            virtual ~sql_operator_builder();

            // equals
            sql_operator_builder &operator=(const sql_value &rvalue);
            // nequals
            sql_operator_builder &operator!=(const sql_value &rvalue);
            // like
            sql_operator_builder &operator^=(const std::string &rvalue);
            // equals lesser
            sql_operator_builder &operator<=(const sql_value &rvalue);
            // equals greater
            sql_operator_builder &operator>=(const sql_value &rvalue);
            // lesser
            sql_operator_builder &operator<(const sql_value &rvalue);
            // greater
            sql_operator_builder &operator>(const sql_value &rvalue);
            // contains
            sql_operator_builder &operator[](const std::string &rvalue);
            // in
            sql_operator_builder &operator[](const std::vector<sql_value> &values);
            // between
            sql_operator_builder &operator[](const std::pair<sql_value, sql_value> &values);
            // is
            sql_operator_builder &operator=(const sql_null_type &rvalue);
            // is not
            sql_operator_builder &operator!=(const sql_null_type &rvalue);
        };

        sql_operator_builder operator"" _op(const char *lvalue, size_t len);

        /*!
         * a utility class aimed at making logic where statements
         * ex. where("a = b") || "c == d" && "e == f";
         */
        class where_clause : public sql_generator
        {
           private:
            std::string value_;
            std::vector<where_clause> and_;
            std::vector<where_clause> or_;

            std::string generate_sql() const;
            void set_modified();

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
             * explicit cast operator to sql string
             * @return the sql string
             */
            explicit operator std::string() const;
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
            bool empty() const noexcept;

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

            where_builder &bind(size_t index, const sql_operator &value);
            std::string to_sql(size_t index, const sql_operator &value);

           protected:
            where_builder &bind(size_t index, const sql_value &value);
            where_builder &bind(const std::string &name, const sql_value &value);

           public:
            where_builder(const std::shared_ptr<session_impl> &session, bindable *bindable);
            where_builder(const where_builder &other);
            where_builder(where_builder &&other);
            where_builder &operator=(const where_builder &other);
            where_builder &operator=(where_builder &&other);
            virtual ~where_builder();

            size_t num_of_bindings() const noexcept;

            using sql_generator::to_sql;

            void reset(const sql_operator &value);

            /*!
             * Appends and AND part to this where clause
             * @param value   the sql to append
             */
            where_builder &operator&&(const sql_operator &value);

            /*!
             * Appends and OR part to this where clause
             * @param value   the sql to append
             */
            where_builder &operator||(const sql_operator &value);
        };

        template <typename T>
        class whereable
        {
           public:
            whereable() = default;
            whereable(const whereable &other) = default;
            whereable(whereable &&other) = default;
            virtual ~whereable() = default;
            whereable &operator=(const whereable &other) = default;
            whereable &operator=(whereable &&other) = default;

            virtual where_builder &where() = 0;
            virtual where_builder &where(const sql_operator &value) = 0;
#ifdef ENHANCED_PARAMTER_MAPPING
            virtual T &where(const where_clause &value) = 0;
            virtual where_builder &where(const std::string &sql) = 0;
#endif
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

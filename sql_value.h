#ifndef ARG3_DB_SQL_VALUE_H_
#define ARG3_DB_SQL_VALUE_H_

#include <iostream>
#include <sqlite3.h>
#include "variant.h"

namespace arg3
{
    namespace db
    {
        class sql_blob
        {
        public:
            typedef void (*cleanup_method)(void *);
        private:
            const void *p_;
            size_t s_;
            cleanup_method destruct_;
        public:

            sql_blob(const void *ptr, size_t size, cleanup_method cleanup = SQLITE_STATIC);

            const void *ptr() const;
            size_t size() const;
            cleanup_method destructor() const;
        };

        typedef struct {} sql_null_t;

        extern const sql_null_t sql_null;

        typedef thenewcpp::Variant<sql_null_t, int, int64_t, double, std::string, sql_blob> sql_value;

        std::ostream &operator<<(std::ostream &out, const sql_value &value);

        std::ostream &operator<<(std::ostream &out, const sql_null_t &value);
    }

}

namespace std
{
    string to_string(const arg3::db::sql_value &value);
}


namespace arg3
{
    namespace db
    {
        inline bool operator==(const sql_null_t &a, const sql_null_t &b)
        {
            return true;
        }
        template<typename T>
        class sql_equality_visitor
        {
            T other;
        public:
            typedef bool result_type;

            sql_equality_visitor(T value) : other(value)
            {}

            bool operator()(T value) const
            {
                return value == other;
            }
            template<typename U>
            bool operator()(U value) const
            {
                return std::to_string(value) == std::to_string(other);
            }
        };

        inline bool operator==(const sql_value &value, const std::string &other)
        {
            return thenewcpp::apply_visitor(sql_equality_visitor<std::string>(other), value);
        }

        inline bool operator==(const std::string &other, const sql_value &value)
        {
            return thenewcpp::apply_visitor(sql_equality_visitor<std::string>(other), value);
        }

        inline bool operator==(const sql_value &value, int other)
        {
            return thenewcpp::apply_visitor(sql_equality_visitor<int>(other), value);
        }

        inline bool operator==(int other, const sql_value &value)
        {
            return thenewcpp::apply_visitor(sql_equality_visitor<int>(other), value);
        }

        inline bool operator==(const sql_value &value, int64_t other)
        {
            return thenewcpp::apply_visitor(sql_equality_visitor<int64_t>(other), value);
        }

        inline bool operator==(int64_t other, const sql_value &value)
        {
            return thenewcpp::apply_visitor(sql_equality_visitor<int64_t>(other), value);
        }

        inline bool operator==(const sql_value &value, double other)
        {
            return thenewcpp::apply_visitor(sql_equality_visitor<double>(other), value);
        }

        inline bool operator==(double other, const sql_value &value)
        {
            return thenewcpp::apply_visitor(sql_equality_visitor<double>(other), value);
        }

        inline bool operator==(const sql_value &value, sql_null_t other)
        {
            return thenewcpp::apply_visitor(sql_equality_visitor<sql_null_t>(other), value);
        }
        inline bool operator==(sql_null_t other, const sql_value &value)
        {
            return thenewcpp::apply_visitor(sql_equality_visitor<sql_null_t>(other), value);
        }
    }
}
#endif

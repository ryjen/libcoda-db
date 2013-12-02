#ifndef ARG3_DB_SQL_VALUE_H_
#define ARG3_DB_SQL_VALUE_H_

#include <iostream>
#include <sqlite3.h>
#include "variant.h"

namespace arg3
{
    namespace db
    {
        class sqldb;
        class base_query;

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
            std::string to_string() const;
            bool operator==(const sql_blob &other) const;
        };

        typedef struct {} sql_null_t;

        extern const sql_null_t sql_null;
    }
}

namespace std
{
    string to_string(const arg3::db::sql_null_t &value);
    string to_string(const arg3::db::sql_blob &value);
    string to_string(const std::string &value);
}

namespace arg3
{

    namespace db
    {
        //typedef thenewcpp::Variant<sql_null_t, int, int64_t, double, std::string, sql_blob> sql_value;

        template<typename T>
        class sql_exists_visitor
        {
        public:
            typedef bool result_type;

            sql_exists_visitor()
            {}

            bool operator()(T value) const
            {
                return true;
            }
            template<typename U>
            bool operator()(U value) const
            {
                return false;
            }
        };

        class sql_binding_visitor
        {
            sqldb *db_;
            sqlite3_stmt *stmt_;
            int index_;
        public:
            typedef void result_type;

            sql_binding_visitor(sqldb *db, sqlite3_stmt *stmt, int index);

            void operator()(int value) const;
            void operator()(int64_t value) const;
            void operator()(double value) const;
            void operator()(const std::string &value) const;
            void operator()(const sql_blob &value) const;
            void operator()(const sql_null_t &value) const;
        };

        class query_binding_visitor
        {
            base_query *query_;
            int index_;
        public:
            typedef void result_type;

            query_binding_visitor(base_query *query, int index);

            void operator()(int value) const;
            void operator()(int64_t value) const;
            void operator()(double value) const;
            void operator()(const std::string &value) const;
            void operator()(const sql_blob &value) const;
            void operator()(const sql_null_t &value) const;
        };

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

        class sql_value
        {
            friend std::ostream &operator<<(std::ostream &out, const sql_value &value);
            friend bool operator==(const std::string &other, const sql_value &value);
            friend bool operator==(int other, const sql_value &value);
            friend bool operator==(int64_t other, const sql_value &value);
            friend bool operator==(double other, const sql_value &value);
            friend bool operator==(const sql_null_t &other, const sql_value &value);
            friend bool operator==(const sql_blob &other, const sql_value &value);
        private:
            thenewcpp::Variant<sql_null_t, int, int64_t, double, std::string, sql_blob> value_;
        public:
            sql_value() : value_(sql_null) {}

            template<typename T>
            sql_value(const T &value) : value_(value) {}

            operator std::string() const;

            operator int() const;

            operator int64_t() const;

            operator double() const;

            operator sql_blob() const;

            std::string to_string() const;

            void bind(sqldb *db, sqlite3_stmt *stmt, int index) const
            {
                apply_visitor(sql_binding_visitor(db, stmt, index), value_);
            }

            void bind(base_query *query, int index) const
            {
                apply_visitor(query_binding_visitor(query, index), value_);
            }

            bool operator==(const sql_value &other) const
            {
                return other.to_string() == to_string();
            }

            /* faster check */
            bool operator==(const sql_null_t &other) const
            {
                return apply_visitor(sql_exists_visitor<sql_null_t>(), value_);
            }

            template<typename T>
            bool operator==(const T &other) const
            {
                return thenewcpp::apply_visitor(sql_equality_visitor<T>(other), value_);
            }
        };

        std::ostream &operator<<(std::ostream &out, const sql_value &value);

        std::ostream &operator<<(std::ostream &out, const sql_null_t &value);

        inline bool operator==(const sql_null_t &a, const sql_null_t &b)
        {
            return true;
        }

        class ostream_sql_value_visitor
        {
            std::ostream &out_;
        public:
            typedef void result_type;

            ostream_sql_value_visitor(std::ostream &out) : out_(out)
            {}

            template<typename T>
            void operator()(const T &value) const
            {
                out_ << value;
            }
        };

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
        inline bool operator==(const std::string &other, const sql_value &value)
        {
            return thenewcpp::apply_visitor(sql_equality_visitor<std::string>(other), value.value_);
        }

        inline bool operator==(int other, const sql_value &value)
        {
            return thenewcpp::apply_visitor(sql_equality_visitor<int>(other), value.value_);
        }

        inline bool operator==(int64_t other, const sql_value &value)
        {
            return thenewcpp::apply_visitor(sql_equality_visitor<int64_t>(other), value.value_);
        }

        inline bool operator==(double other, const sql_value &value)
        {
            return thenewcpp::apply_visitor(sql_equality_visitor<double>(other), value.value_);
        }

        inline bool operator==(const sql_null_t &other, const sql_value &value)
        {
            return thenewcpp::apply_visitor(sql_equality_visitor<sql_null_t>(other), value.value_);
        }

        inline bool operator==(const sql_blob &other, const sql_value &value)
        {
            return thenewcpp::apply_visitor(sql_equality_visitor<sql_blob>(other), value.value_);
        }
    }
}
#endif

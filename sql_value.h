#ifndef ARG3_DB_SQL_VALUE_H_
#define ARG3_DB_SQL_VALUE_H_

#include "variant.h"
#include <iostream>
#include <sqlite3.h>

namespace arg3
{
    namespace db
    {
        class sql_blob
        {
        public:
            typedef void (*cleanup_method)(void*);
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

        typedef struct  {} sql_null_t;

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
        template<typename T>
        class sql_value_equality
        {
        private:
            const T & value_;
        public:
            typedef bool result_type;
            bool operator()( T other ) const
            {
                return std::to_string(other) == std::to_string(value_);
            }

            sql_value_equality( const T& value ) : value_(value) {}
        };

        template<typename T>
        inline bool operator==(const sql_value &value, const T &other)
        {
            return thenewcpp::apply_visitor(sql_value_equality<T>(other), value);
        }
    }
}
#endif

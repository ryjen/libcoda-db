#ifndef RJ_DB_SQL_COMMON_H
#define RJ_DB_SQL_COMMON_H


#include <boost/variant/static_visitor.hpp>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>
#include "exception.h"
#include "sql_time.h"
#include "sql_types.h"

namespace rj
{
    namespace db
    {
        std::ostream &operator<<(std::ostream &out, const sql_blob &value);
        std::wostream &operator<<(std::wostream &out, const sql_blob &value);
        std::ostream &operator<<(std::ostream &out, const sql_null_type &null);
        std::wostream &operator<<(std::wostream &out, const sql_null_type &null);

        std::string to_string(const sql_blob &value);
        std::wstring to_wstring(const sql_blob &value);

        std::string to_string(const sql_null_type &value);
        std::wstring to_wstring(const sql_null_type &value);

        namespace helper
        {
            std::string convert_string(const std::wstring &buf);
            std::wstring convert_string(const std::string &buf);
            
            bool is_positive_bool(const sql_string &value);
            bool is_positive_bool(const sql_wstring &value);
            
            bool is_negative_bool(const sql_string &value);
            bool is_negative_bool(const sql_wstring &value);
            
            int is_bool(const sql_string &value);
            int is_bool(const sql_wstring &value);

            sql_number &&to_number(const sql_string &value);
            sql_number &&to_number(const sql_wstring &value);
            sql_number &&to_number(const sql_time &value);

            template <typename T, typename = std::enable_if<is_sql_number<T>::value>>
            class as_number : public boost::static_visitor<T>
            {
               public:
                template <typename V>
                T operator()(const V &value) const
                {
                    return value;
                }
                T operator()(const sql_null_type &value) const
                {
                    return 0;
                }
                T operator()(const sql_blob &value) const
                {
                    throw value_conversion_error();
                }
                T operator()(const sql_time &value) const
                {
                    if (std::is_same<T, time_t>::value || std::is_convertible<time_t, T>::value) {
                        return to_number(value);
                    }
                    throw value_conversion_error();
                }
                T operator()(const sql_string &value) const
                {
                    return to_number(value);
                }
                T operator()(const sql_wstring &value) const
                {
                    return to_number(value);
                }
                T operator()(const sql_number &value) const
                {
                    return value;
                }
            };


            template <typename T>
            struct is_type : public boost::static_visitor<bool> {
               public:
                template <typename V>
                bool operator()(const V &value) const
                {
                    return std::is_same<T, V>::value || std::is_convertible<V, T>::value;
                }
            };
            

            struct number_equality : public boost::static_visitor<bool> {
            public:
                number_equality(const sql_number &num) : num_(num) {}
                template <typename V>
                bool operator()(const V &value) const
                {
                    return num_ == value;
                }
            private:
                const sql_number &num_;
            };
            
            struct value_equality : public boost::static_visitor<bool> {
            public:
                value_equality(const sql_value &value) : value_(value) {}
                template <typename V>
                bool operator()(const V &value) const
                {
                    return value_ == value;
                }
            private:
                const sql_value &value_;
            };

            class as_sql_string : public boost::static_visitor<sql_string>
            {
               public:
                template <typename V>
                sql_string operator()(const V &value) const
                {
                    return std::to_string(value);
                }

                sql_string operator()(const sql_time &value) const;
                sql_string operator()(const sql_string &value) const;
                sql_string operator()(const sql_wstring &value) const;
                sql_string operator()(const sql_blob &value) const;
                sql_string operator()(const sql_null_type &null) const;
                sql_string operator()(const sql_number &value) const;
            };

            class as_sql_wstring : public boost::static_visitor<sql_wstring>
            {
               public:
                template <typename V>
                sql_wstring operator()(const V &value) const
                {
                    return std::to_wstring(value);
                }

                sql_wstring operator()(const sql_time &value) const;
                sql_wstring operator()(const sql_string &value) const;
                sql_wstring operator()(const sql_wstring &value) const;
                sql_wstring operator()(const sql_blob &value) const;
                sql_wstring operator()(const sql_null_type &null) const;
                sql_wstring operator()(const sql_number &value) const;
            };
        }
    }
}
#endif

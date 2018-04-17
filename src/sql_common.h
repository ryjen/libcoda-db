#ifndef CODA_DB_SQL_COMMON_H
#define CODA_DB_SQL_COMMON_H


#include <boost/variant/static_visitor.hpp>
#include <iterator>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>
#include "sql_types.h"

namespace coda {
    namespace db {
        class sql_time;

        class sql_value;

        // stream operators for different types
        std::ostream &operator<<(std::ostream &out, const sql_blob &value);

        std::wostream &operator<<(std::wostream &out, const sql_blob &value);

        std::ostream &operator<<(std::ostream &out, const sql_null_type &null);

        std::wostream &operator<<(std::wostream &out, const sql_null_type &null);

        // to string functions
        std::string to_string(const sql_blob &value);

        std::wstring to_wstring(const sql_blob &value);

        std::string to_string(const sql_null_type &value);

        std::wstring to_wstring(const sql_null_type &value);

        namespace helper {
            /*!
             * utility method used in creating sql
             */
            template<typename T>
            std::string join_csv(const std::vector<T> &list) {
                std::ostringstream buf;

                if (list.size() > 0) {
                    std::ostream_iterator<T> it(buf, ",");

                    copy(list.begin(), list.end() - 1, it);

                    buf << *(list.end() - 1);
                }

                return buf.str();
            }

            // convert between different string types
            std::string convert_string(const std::wstring &buf);

            std::wstring convert_string(const std::string &buf);

            // test if a string value is a positive boolean value
            bool is_positive_bool(const sql_string &value);

            bool is_positive_bool(const sql_wstring &value);

            // test if a string value is a negative bool value
            bool is_negative_bool(const sql_string &value);

            bool is_negative_bool(const sql_wstring &value);

            /**
             * test if a string value is a positive or negative bool value
             * @returns 0 if not a boolean, -1 if false, 1 if true
             */
            template<typename S, typename = std::enable_if<is_sql_string<S>::value>>
            int is_bool(const S &value) {
                if (is_positive_bool(value)) {
                    return 1;
                }
                if (is_negative_bool(value)) {
                    return -1;
                }
                return 0;
            }

            template<typename T, typename = std::enable_if<is_sql_number<T>::value>>
            class as_number;

            template<typename T>
            struct is_type : public boost::static_visitor<bool> {
            public:
                template<typename V>
                bool operator()(const V &value) const {
                    return std::is_same<T, V>::value || std::is_convertible<V, T>::value;
                }
            };


            struct number_equality : public boost::static_visitor<bool> {
            public:
                number_equality(const sql_number &num) : num_(num) {
                }

                template<typename V>
                bool operator()(const V &value) const {
                    return num_ == value;
                }

            private:
                const sql_number &num_;
            };

            struct value_equality : public boost::static_visitor<bool> {
            public:
                value_equality(const sql_value &value) : value_(value) {
                }

                template<typename V>
                bool operator()(const V &value) const {
                    return value_ == value;
                }

            private:
                const sql_value &value_;
            };

            class as_sql_string : public boost::static_visitor<sql_string> {
            public:
                template<typename V>
                sql_string operator()(const V &value) const {
                    return std::to_string(value);
                }

                sql_string operator()(const sql_time &value) const;

                sql_string operator()(const sql_string &value) const;

                sql_string operator()(const sql_wstring &value) const;

                sql_string operator()(const sql_blob &value) const;

                sql_string operator()(const sql_null_type &null) const;

                sql_string operator()(const sql_number &value) const;
            };

            class as_sql_wstring : public boost::static_visitor<sql_wstring> {
            public:
                template<typename V>
                sql_wstring operator()(const V &value) const {
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

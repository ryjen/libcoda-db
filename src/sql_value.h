/*!
 * @file sql_value.h
 * a value in a column
 */
#ifndef RJ_DB_SQL_VALUE_H
#define RJ_DB_SQL_VALUE_H

#include <type_traits>
#include "exception.h"
#include "sql_number.h"
#include "sql_time.h"

namespace rj
{
    namespace db
    {
        /*!
         * A sql value
         */
        class sql_value : public sql_value_convertible
        {
           public:
            sql_value();
            sql_value(const sql_null_type &value);
            sql_value(const sql_number &value);
            sql_value(const sql_string &value);
            sql_value(const sql_wstring &value);
            sql_value(const sql_time &value);
            sql_value(const sql_blob &value);

            /**
             * numeric constructors
             * would be nice to use a template here, but it can't deduce the type very well
             */
            sql_value(const bool &value);
            sql_value(const char &value);
            sql_value(const unsigned char &value);
            sql_value(const wchar_t &value);
            sql_value(const short &value);
            sql_value(const unsigned short &value);
            sql_value(const int &value);
            sql_value(const unsigned int &value);
            sql_value(const long &value);
            sql_value(const unsigned long &value);
            sql_value(const long long &value);
            sql_value(const unsigned long long &value);
            sql_value(const float &value);
            sql_value(const double &value);
            sql_value(const long double &value);

            template <typename T, typename = std::enable_if<is_sql_value<T>::value || is_sql_number<T>::value>>
            bool is() const
            {
                return boost::apply_visitor(helper::is_type<T>(), value_);
            }

            template <typename T, typename = std::enable_if<is_sql_number<T>::value>>
            T as() const
            {
                return boost::apply_visitor(helper::as_number<T>(), value_);
            }

            operator sql_null_type() const;
            operator sql_number() const;
            operator sql_string() const;
            operator sql_wstring() const;
            operator sql_time() const;
            operator sql_blob() const;

            /* numeric implicits */
            operator bool() const;
            operator char() const;
            operator unsigned char() const;
            operator wchar_t() const;
            operator short() const;
            operator unsigned short() const;
            operator int() const;
            operator unsigned int() const;
            operator long() const;
            operator unsigned long() const;
            operator long long() const;
            operator unsigned long long() const;
            operator float() const;
            operator double() const;
            operator long double() const;

            bool operator==(const sql_value &other) const;

            bool operator==(const sql_null_type &other) const;
            bool operator==(const sql_number &value) const;
            bool operator==(const sql_string &value) const;
            bool operator==(const sql_wstring &value) const;
            bool operator==(const sql_time &value) const;
            bool operator==(const sql_blob &value) const;

            /* numeric equality */
            bool operator==(const bool &value) const;
            bool operator==(const char &value) const;
            bool operator==(const unsigned char &value) const;
            bool operator==(const wchar_t &value) const;
            bool operator==(const short &value) const;
            bool operator==(const unsigned short &value) const;
            bool operator==(const int &value) const;
            bool operator==(const unsigned int &value) const;
            bool operator==(const long &value) const;
            bool operator==(const unsigned long &value) const;
            bool operator==(const long long &value) const;
            bool operator==(const unsigned long long &value) const;
            bool operator==(const float &value) const;
            bool operator==(const double &value) const;
            bool operator==(const long double &value) const;

            // template <typename T>
            // bool operator==(const typename std::enable_if<is_sql_number<T>::value, T>::type &other) const;

            std::string to_string() const;
            std::wstring to_wstring() const;

            template <typename V, typename T>
            T apply_visitor(const V &visitor) const
            {
                return boost::apply_visitor(visitor, value_);
            }
            template <typename V>
            void apply_visitor(const V &visitor) const
            {
                boost::apply_visitor(visitor, value_);
            }

           private:
            boost::variant<sql_null_type, sql_number, sql_string, sql_wstring, sql_time, sql_blob> value_;
        };

        template <>
        sql_string sql_value::as<sql_string>() const;

        template <>
        sql_wstring sql_value::as<sql_wstring>() const;

        template <>
        sql_time sql_value::as<sql_time>() const;

        template <>
        sql_blob sql_value::as<sql_blob>() const;

        template <>
        sql_number sql_value::as<sql_number>() const;

        /*!
         * output stream operator for a sql null type
         * will append "NULL"
         * @param out  the output stream
         * @param null  the null type to append
         */
        std::ostream &operator<<(std::ostream &out, const sql_value &value);

        bool operator==(const sql_null_type &null, const sql_value &value);
        bool operator==(const sql_time &time, const sql_value &value);
        bool operator==(const sql_string &other, const sql_value &value);
        bool operator==(const sql_wstring &other, const sql_value &value);
        bool operator==(const sql_number &other, const sql_value &value);
        bool operator==(const sql_blob &other, const sql_value &value);

        /* numeric equality */
        bool operator==(const bool &value, const sql_value &other);
        bool operator==(const char &value, const sql_value &other);
        bool operator==(const unsigned char &value, const sql_value &other);
        bool operator==(const wchar_t &value, const sql_value &other);
        bool operator==(const short &value, const sql_value &other);
        bool operator==(const unsigned short &value, const sql_value &other);
        bool operator==(const int &value, const sql_value &other);
        bool operator==(const unsigned int &value, const sql_value &other);
        bool operator==(const long &value, const sql_value &other);
        bool operator==(const unsigned long &value, const sql_value &other);
        bool operator==(const long long &value, const sql_value &other);
        bool operator==(const unsigned long long &value, const sql_value &other);
        bool operator==(const float &value, const sql_value &other);
        bool operator==(const double &value, const sql_value &other);
        bool operator==(const long double &value, const sql_value &other);
    }
}
// namespace std
// {
//     /*!
//      * get a string representation of a sql null
//      * @param  null the null type
//      * @return      a NULL sql string
//      */
//     std::string to_string(const rj::db::sql_null_type &null);
//     std::wstring to_wstring(const rj::db::sql_null_type &null);

//     std::string to_string(const rj::db::sql_blob &value);
//     std::wstring to_wstring(const rj::db::sql_blob &value);
// }

#endif

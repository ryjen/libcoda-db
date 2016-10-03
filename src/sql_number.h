
#ifndef RJ_DB_SQL_NUMBER_H
#define RJ_DB_SQL_NUMBER_H

#include <boost/variant.hpp>
#include <string>
#include <vector>
#include "exception.h"
#include "sql_common.h"
#include "sql_time.h"
#include "sql_types.h"

namespace rj
{
    namespace db
    {
        class sql_number : public sql_number_convertible
        {
           public:
            sql_number();

            sql_number(const bool &value);
            sql_number(const char &value);
            sql_number(const unsigned char &value);
            sql_number(const wchar_t &value);
            sql_number(const short &value);
            sql_number(const unsigned short &value);
            sql_number(const int &value);
            sql_number(const unsigned int &value);
            sql_number(const long &value);
            sql_number(const unsigned long &value);
            sql_number(const long long &value);
            sql_number(const unsigned long long &value);
            sql_number(const float &value);
            sql_number(const double &value);
            sql_number(const long double &value);

            sql_number(const sql_string &value);
            sql_number(const sql_wstring &value);

            sql_number(const sql_null_type &value);
            sql_number(const sql_time &value);

            sql_number(const sql_number &other);
            sql_number(sql_number &&other);
            sql_number &operator=(const sql_number &other);
            sql_number &operator=(sql_number &&other);
            virtual ~sql_number();

            template <typename T, typename = std::enable_if<is_sql_number<T>::value>>
            T as() const
            {
                return boost::apply_visitor(helper::as_number<T>(), value_);
            }

            /**
             * not a template because it some cases it can't deduce the type
             */
            operator sql_null_type() const;
            operator sql_string() const;
            operator sql_wstring() const;
            operator sql_time() const;

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

            std::string to_string() const;
            std::wstring to_wstring() const;

            bool parse(const std::string &value);
            bool parse(const std::wstring &value);


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

            bool operator==(const sql_number &other) const;
            bool operator==(const sql_null_type &other) const;
            bool operator==(const sql_string &value) const;
            bool operator==(const sql_wstring &value) const;
            bool operator==(const sql_time &value) const;

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

           private:
            constexpr static const int BASE10 = 10;

            template <typename T>
            typename std::enable_if<std::is_integral<T>::value, bool>::type parse_integral(const std::string &value,
                                                                                           T (*funk)(const std::string &, size_t *, int),
                                                                                           int base = BASE10)
            {
                try {
                    value_ = funk(value, nullptr, base);
                    return true;
                } catch (const std::exception &e) {
                    return false;
                }
            }

            template <typename T>
            typename std::enable_if<std::is_integral<T>::value, bool>::type parse_integral(const std::wstring &value,
                                                                                           T (*funk)(const std::wstring &, size_t *, int),
                                                                                           int base = BASE10)
            {
                try {
                    value_ = funk(value, nullptr, base);
                    return true;
                } catch (const std::exception &e) {
                    return false;
                }
            }

            template <typename T>
            typename std::enable_if<std::is_floating_point<T>::value, bool>::type parse_floating(const std::string &value,
                                                                                                 T (*funk)(const std::string &, size_t *))
            {
                try {
                    value_ = funk(value, nullptr);
                    return true;
                } catch (const std::exception &e) {
                    return false;
                }
            }
            template <typename T>
            typename std::enable_if<std::is_floating_point<T>::value, bool>::type parse_floating(const std::wstring &value,
                                                                                                 T (*funk)(const std::wstring &, size_t *))
            {
                try {
                    value_ = funk(value, nullptr);
                    return true;
                } catch (const std::exception &e) {
                    return false;
                }
            }

            boost::variant<sql_null_type, bool, char, unsigned char, wchar_t, short, unsigned short, int, unsigned int, long, unsigned long,
                           long long, unsigned long long, float, double, long double>
                value_;
        };

        template <>
        sql_string sql_number::as<sql_string>() const;

        template <>
        sql_wstring sql_number::as() const;

        template <>
        sql_time sql_number::as() const;

        std::ostream &operator<<(std::ostream &out, const sql_number &value);
    }
}

#endif

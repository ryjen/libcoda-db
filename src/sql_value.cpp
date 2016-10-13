
#include "sql_value.h"
#include <cwchar>
#include <sstream>
#include "exception.h"
#include "log.h"
#include "query.h"
#include "sql_common.h"
#include "sqldb.h"

using namespace std;

namespace rj
{
    namespace db
    {
        namespace helper
        {
            struct as_sql_time : public boost::static_visitor<sql_time> {
               public:
                as_sql_time()
                {
                }

                sql_time operator()(const sql_number &value) const
                {
                    return value;
                }
                sql_time operator()(const sql_time &value) const
                {
                    return value;
                }
                sql_time operator()(const sql_blob &value) const
                {
                    throw value_conversion_error();
                }
                sql_time operator()(const sql_string &value) const
                {
                    return sql_time(value);
                }
                sql_time operator()(const sql_wstring &value) const
                {
                    return sql_number(value);
                }
                sql_time operator()(const sql_null_type &value) const
                {
                    throw value_conversion_error();
                }

               private:
                bool throw_;
            };

            struct as_sql_blob : public boost::static_visitor<sql_blob> {
               public:
                sql_blob operator()(const sql_number &value) const
                {
                    throw value_conversion_error();
                }
                sql_blob operator()(const sql_time &value) const
                {
                    throw value_conversion_error();
                }
                sql_blob operator()(const sql_blob &value) const
                {
                    return value;
                }
                sql_blob operator()(const sql_string &value) const
                {
                    return sql_blob(value.begin(), value.end());
                }
                sql_blob operator()(const sql_wstring &value) const
                {
                    return sql_blob(value.begin(), value.end());
                }
                sql_blob operator()(const sql_null_type &value) const
                {
                    return sql_blob();
                }
            };
            struct as_sql_number : public boost::static_visitor<sql_number> {
               public:
                sql_number operator()(const sql_number &value) const
                {
                    return value;
                }
                sql_number operator()(const sql_time &value) const
                {
                    return value.value();
                }
                sql_number operator()(const sql_blob &value) const
                {
                    return value.size();
                }
                sql_number operator()(const sql_string &value) const
                {
                    return sql_number(value);
                }
                sql_number operator()(const sql_wstring &value) const
                {
                    return sql_number(value);
                }
                sql_number operator()(const sql_null_type &value) const
                {
                    return sql_number(value);
                }
            };
        }
        const nullptr_t sql_null = nullptr;

        sql_value::sql_value() : value_(sql_null)
        {
        }

        sql_value::sql_value(const sql_null_type &value) : value_(value)
        {
        }
        sql_value::sql_value(const sql_number &value) : value_(value)
        {
        }
        sql_value::sql_value(const sql_string &value) : value_(value)
        {
        }
        sql_value::sql_value(const sql_wstring &value) : value_(value)
        {
        }
        sql_value::sql_value(const sql_time &value) : value_(value)
        {
        }
        sql_value::sql_value(const sql_blob &value) : value_(value)
        {
        }

        sql_value::sql_value(const bool &value) : value_(sql_number(value))
        {
        }
        sql_value::sql_value(const char &value) : value_(sql_number(value))
        {
        }
        sql_value::sql_value(const unsigned char &value) : value_(sql_number(value))
        {
        }
        sql_value::sql_value(const wchar_t &value) : value_(sql_number(value))
        {
        }
        sql_value::sql_value(const short &value) : value_(sql_number(value))
        {
        }
        sql_value::sql_value(const unsigned short &value) : value_(sql_number(value))
        {
        }
        sql_value::sql_value(const int &value) : value_(sql_number(value))
        {
        }
        sql_value::sql_value(const unsigned int &value) : value_(sql_number(value))
        {
        }
        sql_value::sql_value(const long &value) : value_(sql_number(value))
        {
        }
        sql_value::sql_value(const unsigned long &value) : value_(sql_number(value))
        {
        }
        sql_value::sql_value(const long long &value) : value_(sql_number(value))
        {
        }
        sql_value::sql_value(const unsigned long long &value) : value_(sql_number(value))
        {
        }
        sql_value::sql_value(const float &value) : value_(sql_number(value))
        {
        }
        sql_value::sql_value(const double &value) : value_(sql_number(value))
        {
        }
        sql_value::sql_value(const long double &value) : value_(sql_number(value))
        {
        }
        sql_value::sql_value(const char *value) : value_(sql_string(value))
        {
        }
        sql_value::sql_value(const wchar_t *value) : value_(sql_wstring(value))
        {
        }

        template <>
        sql_string sql_value::as() const
        {
            return boost::apply_visitor(helper::as_sql_string(), value_);
        }
        template <>
        sql_wstring sql_value::as() const
        {
            return boost::apply_visitor(helper::as_sql_wstring(), value_);
        }
        template <>
        sql_time sql_value::as() const
        {
            return boost::apply_visitor(helper::as_sql_time(), value_);
        }
        template <>
        sql_blob sql_value::as() const
        {
            return boost::apply_visitor(helper::as_sql_blob(), value_);
        }
        template <>
        sql_number sql_value::as() const
        {
            return boost::apply_visitor(helper::as_sql_number(), value_);
        }

        sql_value::operator sql_number() const
        {
            try {
                return as<sql_number>();
            } catch (const value_conversion_error &e) {
                return sql_number();
            }
        }
        sql_value::operator sql_string() const
        {
            try {
                return as<sql_string>();
            } catch (const value_conversion_error &e) {
                return sql_string();
            }
        }
        sql_value::operator sql_wstring() const
        {
            try {
                return as<sql_wstring>();
            } catch (const value_conversion_error &e) {
                return sql_wstring();
            }
        }
        sql_value::operator sql_time() const
        {
            try {
                return as<sql_time>();
            } catch (const value_conversion_error &e) {
                return sql_time();
            }
        }
        sql_value::operator sql_blob() const
        {
            try {
                return as<sql_blob>();
            } catch (const value_conversion_error &e) {
                return sql_blob();
            }
        }

        sql_value::operator bool() const
        {
            try {
                return as<sql_number>();
            } catch (const value_conversion_error &e) {
                return false;
            }
        }
        sql_value::operator char() const
        {
            try {
                return as<sql_number>();
            } catch (const value_conversion_error &e) {
                return 0;
            }
        }
        sql_value::operator unsigned char() const
        {
            try {
                return as<sql_number>();
            } catch (const value_conversion_error &e) {
                return 0;
            }
        }
        sql_value::operator wchar_t() const
        {
            try {
                return as<sql_number>();
            } catch (const value_conversion_error &e) {
                return 0;
            }
        }
        sql_value::operator short() const
        {
            try {
                return as<sql_number>();
            } catch (const value_conversion_error &e) {
                return 0;
            }
        }
        sql_value::operator unsigned short() const
        {
            try {
                return as<sql_number>();
            } catch (const value_conversion_error &e) {
                return 0;
            }
        }
        sql_value::operator int() const
        {
            try {
                return as<sql_number>();
            } catch (const value_conversion_error &e) {
                return 0;
            }
        }
        sql_value::operator unsigned int() const
        {
            try {
                return as<sql_number>();
            } catch (const value_conversion_error &e) {
                return 0;
            }
        }
        sql_value::operator long() const
        {
            try {
                return as<sql_number>();
            } catch (const value_conversion_error &e) {
                return 0;
            }
        }
        sql_value::operator unsigned long() const
        {
            try {
                return as<sql_number>();
            } catch (const value_conversion_error &e) {
                return 0;
            }
        }
        sql_value::operator long long() const
        {
            try {
                return as<sql_number>();
            } catch (const value_conversion_error &e) {
                return 0;
            }
        }
        sql_value::operator unsigned long long() const
        {
            try {
                return as<sql_number>();
            } catch (const value_conversion_error &e) {
                return 0;
            }
        }
        sql_value::operator float() const
        {
            try {
                return as<sql_number>();
            } catch (const value_conversion_error &e) {
                return 0;
            }
        }
        sql_value::operator double() const
        {
            try {
                return as<sql_number>();
            } catch (const value_conversion_error &e) {
                return 0;
            }
        }
        sql_value::operator long double() const
        {
            try {
                return as<sql_number>();
            } catch (const value_conversion_error &e) {
                return 0;
            }
        }
        bool sql_value::operator==(const sql_value &value) const
        {
            return boost::apply_visitor(helper::value_equality(value), value_);
        }

        bool sql_value::operator==(const sql_null_type &value) const
        {
            return is<sql_null_type>();
        }

        bool sql_value::operator==(const sql_number &value) const
        {
            try {
                return as<sql_number>() == value;
            } catch (const value_conversion_error &e) {
                return false;
            }
        }
        bool sql_value::operator==(const sql_string &value) const
        {
            try {
                return as<sql_string>() == value;
            } catch (const value_conversion_error &e) {
                return false;
            }
        }
        bool sql_value::operator==(const sql_wstring &value) const
        {
            try {
                return as<sql_wstring>() == value;
            } catch (const value_conversion_error &e) {
                return false;
            }
        }
        bool sql_value::operator==(const sql_time &value) const
        {
            try {
                return as<sql_time>() == value;
            } catch (const value_conversion_error &e) {
                return false;
            }
        }
        bool sql_value::operator==(const sql_blob &value) const
        {
            try {
                return as<sql_blob>() == value;
            } catch (const value_conversion_error &e) {
                return false;
            }
        }

        /* numeric equality */
        bool sql_value::operator==(const bool &value) const
        {
            try {
                return as<sql_number>() == value;
            } catch (const value_conversion_error &e) {
                return false == value;
            }
        }
        bool sql_value::operator==(const char &value) const
        {
            try {
                return as<sql_number>() == value;
            } catch (const value_conversion_error &e) {
                return false;
            }
        }
        bool sql_value::operator==(const unsigned char &value) const
        {
            try {
                return as<sql_number>() == value;
            } catch (const value_conversion_error &e) {
                return false;
            }
        }
        bool sql_value::operator==(const wchar_t &value) const
        {
            try {
                return as<sql_number>() == value;
            } catch (const value_conversion_error &e) {
                return false;
            }
        }
        bool sql_value::operator==(const short &value) const
        {
            try {
                return as<sql_number>() == value;
            } catch (const value_conversion_error &e) {
                return false;
            }
        }
        bool sql_value::operator==(const unsigned short &value) const
        {
            try {
                return as<sql_number>() == value;
            } catch (const value_conversion_error &e) {
                return false;
            }
        }
        bool sql_value::operator==(const int &value) const
        {
            try {
                return as<sql_number>() == value;
            } catch (const value_conversion_error &e) {
                return false;
            }
        }
        bool sql_value::operator==(const unsigned int &value) const
        {
            try {
                return as<sql_number>() == value;
            } catch (const value_conversion_error &e) {
                return false;
            }
        }
        bool sql_value::operator==(const long &value) const
        {
            try {
                return as<sql_number>() == value;
            } catch (const value_conversion_error &e) {
                return false;
            }
        }
        bool sql_value::sql_value::operator==(const unsigned long &value) const
        {
            try {
                return as<sql_number>() == value;
            } catch (const value_conversion_error &e) {
                return false;
            }
        }
        bool sql_value::operator==(const long long &value) const
        {
            try {
                return as<sql_number>() == value;
            } catch (const value_conversion_error &e) {
                return false;
            }
        }
        bool sql_value::operator==(const unsigned long long &value) const
        {
            try {
                return as<sql_number>() == value;
            } catch (const value_conversion_error &e) {
                return false;
            }
        }
        bool sql_value::operator==(const float &value) const
        {
            try {
                return as<sql_number>() == value;
            } catch (const value_conversion_error &e) {
                return false;
            }
        }
        bool sql_value::operator==(const double &value) const
        {
            try {
                return as<sql_number>() == value;
            } catch (const value_conversion_error &e) {
                return false;
            }
        }
        bool sql_value::operator==(const long double &value) const
        {
            try {
                return as<sql_number>() == value;
            } catch (const value_conversion_error &e) {
                return false;
            }
        }

        std::string sql_value::to_string() const
        {
            try {
                return as<sql_string>();
            } catch (const value_conversion_error &e) {
                return std::string();
            }
        }

        ostream &operator<<(ostream &out, const sql_value &value)
        {
            out << value.to_string();
            return out;
        }


        bool operator==(const sql_null_type &null, const sql_value &value)
        {
            return value.operator==(null);
        }

        bool operator==(const sql_string &other, const sql_value &value)
        {
            return value.operator==(other);
        }
        bool operator==(const sql_number &other, const sql_value &value)
        {
            return value.operator==(other);
        }

        bool operator==(const sql_time &time, const sql_value &value)
        {
            return value.operator==(time);
        }
        bool operator==(const sql_wstring &other, const sql_value &value)
        {
            return value.operator==(other);
        }
        bool operator==(const sql_blob &other, const sql_value &value)
        {
            return value.operator==(other);
        }

        /* numeric equality */
        bool operator==(const bool &value, const sql_value &other)
        {
            return other.operator==(value);
        }
        bool operator==(const char &value, const sql_value &other)
        {
            return other.operator==(value);
        }
        bool operator==(const unsigned char &value, const sql_value &other)
        {
            return other.operator==(value);
        }
        bool operator==(const wchar_t &value, const sql_value &other)
        {
            return other.operator==(value);
        }
        bool operator==(const short &value, const sql_value &other)
        {
            return other.operator==(value);
        }
        bool operator==(const unsigned short &value, const sql_value &other)
        {
            return other.operator==(value);
        }
        bool operator==(const int &value, const sql_value &other)
        {
            return other.operator==(value);
        }
        bool operator==(const unsigned int &value, const sql_value &other)
        {
            return other.operator==(value);
        }
        bool operator==(const long &value, const sql_value &other)
        {
            return other.operator==(value);
        }
        bool operator==(const unsigned long &value, const sql_value &other)
        {
            return other.operator==(value);
        }
        bool operator==(const long long &value, const sql_value &other)
        {
            return other.operator==(value);
        }
        bool operator==(const unsigned long long &value, const sql_value &other)
        {
            return other.operator==(value);
        }
        bool operator==(const float &value, const sql_value &other)
        {
            return other.operator==(value);
        }
        bool operator==(const double &value, const sql_value &other)
        {
            return other.operator==(value);
        }
        bool operator==(const long double &value, const sql_value &other)
        {
            return other.operator==(value);
        }
    }
}

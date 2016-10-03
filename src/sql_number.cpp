#include "sql_number.h"
#include "exception.h"
#include "sql_common.h"

namespace rj
{
    namespace db
    {
        sql_number::sql_number() : value_(0)
        {
        }

        sql_number::sql_number(const bool &value) : value_(value)
        {
        }
        sql_number::sql_number(const char &value) : value_(value)
        {
        }
        sql_number::sql_number(const unsigned char &value) : value_(value)
        {
        }
        sql_number::sql_number(const wchar_t &value) : value_(value)
        {
        }
        sql_number::sql_number(const short &value) : value_(value)
        {
        }
        sql_number::sql_number(const unsigned short &value) : value_(value)
        {
        }
        sql_number::sql_number(const int &value) : value_(value)
        {
        }
        sql_number::sql_number(const unsigned int &value) : value_(value)
        {
        }
        sql_number::sql_number(const long &value) : value_(value)
        {
        }
        sql_number::sql_number(const unsigned long &value) : value_(value)
        {
        }
        sql_number::sql_number(const long long &value) : value_(value)
        {
        }
        sql_number::sql_number(const unsigned long long &value) : value_(value)
        {
        }
        sql_number::sql_number(const float &value) : value_(value)
        {
        }
        sql_number::sql_number(const double &value) : value_(value)
        {
        }
        sql_number::sql_number(const long double &value) : value_(value)
        {
        }
        sql_number::sql_number(const sql_string &value)
        {
            if (!parse(value)) {
                throw value_conversion_error();
            }
        }
        sql_number::sql_number(const sql_wstring &value)
        {
            if (!parse(value)) {
                throw value_conversion_error();
            }
        }
        sql_number::sql_number(const sql_time &value) : value_(value.value())
        {
        }

        sql_number::sql_number(const sql_null_type &value) : value_(value)
        {
        }

        sql_number::sql_number(const sql_number &other) : value_(other.value_)
        {
        }
        sql_number::sql_number(sql_number &&other) : value_(std::move(other.value_))
        {
        }
        sql_number &sql_number::operator=(const sql_number &other)
        {
            value_ = other.value_;
            return *this;
        }
        sql_number &sql_number::operator=(sql_number &&other)
        {
            value_ = std::move(other.value_);
            return *this;
        }
        sql_number::~sql_number()
        {
        }


        bool sql_number::parse(const std::string &value)
        {
            if (!std::any_of(value.begin(), value.end(), ::isdigit)) {
                return false;
            }
            if (value.find('.') != std::string::npos) {
                return parse_floating<float>(value, std::stof) || parse_floating<double>(value, std::stod) ||
                       parse_floating<long double>(value, std::stold);
            }
            return parse_integral<int>(value, std::stoi) || parse_integral<long>(value, std::stol) ||
                   parse_integral<unsigned long>(value, std::stoul) || parse_integral<long long>(value, std::stoll) ||
                   parse_integral<unsigned long long>(value, std::stoull);
        }

        bool sql_number::parse(const std::wstring &value)
        {
            if (!std::any_of(value.begin(), value.end(), ::isdigit)) {
                return false;
            }
            if (value.find('.') != std::string::npos) {
                return parse_floating<float>(value, std::stof) || parse_floating<double>(value, std::stod) ||
                       parse_floating<long double>(value, std::stold);
            }
            return parse_integral<int>(value, std::stoi) || parse_integral<long>(value, std::stol) ||
                   parse_integral<unsigned long>(value, std::stoul) || parse_integral<long long>(value, std::stoll) ||
                   parse_integral<unsigned long long>(value, std::stoull);
        }

        template <>
        sql_string sql_number::as() const
        {
            return boost::apply_visitor(helper::as_sql_string(), value_);
        }
        template <>
        sql_wstring sql_number::as() const
        {
            return boost::apply_visitor(helper::as_sql_wstring(), value_);
        }
        template <>
        sql_time sql_number::as() const
        {
            return sql_time(as<long>());
        }

        sql_number::operator sql_null_type() const
        {
            return boost::get<sql_null_type>(value_);
        }

        sql_number::operator bool() const
        {
            return as<bool>();
        }
        sql_number::sql_number::operator char() const
        {
            return as<char>();
        }
        sql_number::operator unsigned char() const
        {
            return as<unsigned char>();
        }
        sql_number::operator wchar_t() const
        {
            return as<wchar_t>();
        }
        sql_number::operator short() const
        {
            return as<short>();
        }
        sql_number::operator unsigned short() const
        {
            return as<unsigned short>();
        }
        sql_number::operator int() const
        {
            return as<int>();
        }
        sql_number::operator unsigned int() const
        {
            return as<unsigned int>();
        }
        sql_number::operator long() const
        {
            return as<long>();
        }
        sql_number::operator unsigned long() const
        {
            return as<unsigned long>();
        }
        sql_number::operator long long() const
        {
            return as<long long>();
        }
        sql_number::operator unsigned long long() const
        {
            return as<unsigned long long>();
        }
        sql_number::operator float() const
        {
            return as<float>();
        }
        sql_number::operator double() const
        {
            return as<double>();
        }
        sql_number::operator long double() const
        {
            return as<long double>();
        }
        sql_number::operator sql_string() const
        {
            return boost::apply_visitor(helper::as_sql_string(), value_);
        }
        sql_number::operator sql_wstring() const
        {
            return boost::apply_visitor(helper::as_sql_wstring(), value_);
        }
        sql_number::operator sql_time() const
        {
            return as<sql_time>();
        }

        bool sql_number::operator==(const sql_number &other) const
        {
            return value_ == other.value_;
        }

        bool sql_number::operator==(const sql_null_type &value) const
        {
            try {
                boost::get<sql_null_type>(value_);
                return true;
            } catch (const boost::bad_get &e) {
                return false;
            }
        }
        bool sql_number::operator==(const sql_string &value) const
        {
            return as<sql_string>() == value;
        }
        bool sql_number::operator==(const sql_wstring &value) const
        {
            return as<sql_wstring>() == value;
        }
        bool sql_number::operator==(const sql_time &value) const
        {
            return as<time_t>() == value.value();
        }

        /* numeric equality */
        bool sql_number::operator==(const bool &value) const
        {
            return as<bool>() == value;
        }
        bool sql_number::operator==(const char &value) const
        {
            return as<char>() == value;
        }
        bool sql_number::operator==(const unsigned char &value) const
        {
            return as<unsigned char>() == value;
        }
        bool sql_number::operator==(const wchar_t &value) const
        {
            return as<wchar_t>() == value;
        }
        bool sql_number::operator==(const short &value) const
        {
            return as<short>() == value;
        }
        bool sql_number::operator==(const unsigned short &value) const
        {
            return as<unsigned short>() == value;
        }
        bool sql_number::operator==(const int &value) const
        {
            return as<int>() == value;
        }
        bool sql_number::operator==(const unsigned int &value) const
        {
            return as<unsigned int>() == value;
        }
        bool sql_number::operator==(const long &value) const
        {
            return as<long>() == value;
        }
        bool sql_number::operator==(const unsigned long &value) const
        {
            return as<unsigned long>() == value;
        }
        bool sql_number::operator==(const long long &value) const
        {
            return as<long long>() == value;
        }
        bool sql_number::operator==(const unsigned long long &value) const
        {
            return as<unsigned long long>() == value;
        }
        bool sql_number::operator==(const float &value) const
        {
            return as<float>() == value;
        }
        bool sql_number::operator==(const double &value) const
        {
            return as<double>() == value;
        }
        bool sql_number::operator==(const long double &value) const
        {
            return as<long double>() == value;
        }

        std::string sql_number::to_string() const
        {
            return boost::apply_visitor(helper::as_sql_string(), value_);
        }
        std::wstring sql_number::to_wstring() const
        {
            return boost::apply_visitor(helper::as_sql_wstring(), value_);
        }

        std::ostream &operator<<(std::ostream &out, const sql_number &value)
        {
            out << value.to_string();
            return out;
        }
    }
}
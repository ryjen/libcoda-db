#include "sql_common.h"
#include <codecvt>
#include <locale>
#include <string>
#include "exception.h"
#include "sql_number.h"

namespace rj
{
    namespace db
    {
        std::ostream &operator<<(std::ostream &out, const sql_blob &value)
        {
            out << std::string(value.begin(), value.end());
            return out;
        }
        std::wostream &operator<<(std::wostream &out, const sql_blob &value)
        {
            out << std::wstring(value.begin(), value.end());
            return out;
        }
        std::ostream &operator<<(std::ostream &out, const sql_null_type &null)
        {
            out << "NULL";
            return out;
        }
        std::wostream &operator<<(std::wostream &out, const sql_null_type &null)
        {
            out << "NULL";
            return out;
        }
        std::string to_string(const sql_blob &value)
        {
            std::ostringstream ss;
            operator<<(ss, value);
            return ss.str();
        }
        std::wstring to_wstring(const sql_blob &value)
        {
            std::wostringstream ss;
            operator<<(ss, value);
            return ss.str();
        }

        std::string to_string(const sql_null_type &value)
        {
            return "NULL";
        }
        std::wstring to_wstring(const sql_null_type &value)
        {
            return L"NULL";
        }

        namespace helper
        {
            bool equals(const std::string &s1, const std::string &s2)
            {
                return ((s1.size() == s2.size()) && std::equal(s1.begin(), s1.end(), s2.begin(), [](char a, char b) {
                            return toupper(a) == toupper(b);
                        }));
            }
            bool equals(const std::wstring &s1, const std::wstring &s2)
            {
                return ((s1.size() == s2.size()) &&
                        std::equal(s1.begin(), s1.end(), s2.begin(),
                                   [](wchar_t a, wchar_t b) { return towupper(a) == towupper(b); }));
            }

            bool is_positive_bool(const sql_string &value)
            {
                return equals(value, "true") || equals(value, "yes") || value == "1";
            }
            bool is_positive_bool(const sql_wstring &value)
            {
                return equals(value, L"true") || equals(value, L"yes") || value == L"1";
            }

            bool is_negative_bool(const sql_string &value)
            {
                return equals(value, "false") || equals(value, "no") || value == "0";
            }
            bool is_negative_bool(const sql_wstring &value)
            {
                return equals(value, L"false") || equals(value, L"no") || value == L"0";
            }

            std::string convert_string(const std::wstring &buf)
            {
                typedef std::codecvt_utf8<wchar_t> convert_type;
                std::wstring_convert<convert_type, wchar_t> converter;
                return converter.to_bytes(buf);
            }

            std::wstring convert_string(const std::string &buf)
            {
                return std::wstring(buf.begin(), buf.end());
            }

            sql_string as_sql_string::operator()(const sql_time &value) const
            {
                return value.to_string();
            }

            sql_string as_sql_string::operator()(const sql_string &value) const
            {
                return value;
            }
            sql_string as_sql_string::operator()(const sql_wstring &value) const
            {
                return convert_string(value);
            }
            sql_string as_sql_string::operator()(const sql_blob &value) const
            {
                std::ostringstream ss;
                operator<<(ss, value);
                return ss.str();
            }

            sql_string as_sql_string::operator()(const sql_null_type &null) const
            {
                return "NULL";
            }
            sql_string as_sql_string::operator()(const sql_number &value) const
            {
                return value;
            }

            sql_wstring as_sql_wstring::operator()(const sql_time &value) const
            {
                return value;
            }

            sql_wstring as_sql_wstring::operator()(const sql_string &value) const
            {
                return convert_string(value);
            }
            sql_wstring as_sql_wstring::operator()(const sql_wstring &value) const
            {
                return value;
            }
            sql_wstring as_sql_wstring::operator()(const sql_blob &value) const
            {
                std::wostringstream ss;
                operator<<(ss, value);
                return ss.str();
            }

            sql_wstring as_sql_wstring::operator()(const sql_null_type &null) const
            {
                return L"NULL";
            }
            sql_wstring as_sql_wstring::operator()(const sql_number &value) const
            {
                return value.to_wstring();
            }
        };
    }
}

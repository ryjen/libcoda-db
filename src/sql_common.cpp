#include "sql_common.h"
#include <codecvt>
#include "sql_number.h"
#include "sql_time.h"

namespace rj
{
    namespace db
    {
        std::ostream &operator<<(std::ostream &out, const sql_blob &value)
        {
            out << std::hex << value.data();
            return out;
        }
        std::wostream &operator<<(std::wostream &out, const sql_blob &value)
        {
            out << std::hex << value.data();
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
            sql_number &&to_number(const sql_string &value)
            {
                return std::move(sql_number(value));
            }
            sql_number &&to_number(const sql_wstring &value)
            {
                return std::move(sql_number(value));
            }
            sql_number &&to_number(const sql_time &value)
            {
                return std::move(sql_number(value.value()));
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
                ss << std::hex << value.data();
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
                ss << std::hex << value.data();
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
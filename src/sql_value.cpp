
#include <sstream>
#include <cwchar>
#include "sql_value.h"
#include "query.h"
#include "sqldb.h"
#include "exception.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        const nullptr_t sql_null = nullptr;

        sql_time::sql_time() : value_(0), format_(TIMESTAMP)
        {
        }

        sql_time::sql_time(time_t value, formats format) : value_(value), format_(format)
        {
        }

        size_t sql_time::size() const
        {
            return sizeof(time_t);
        }

        long sql_time::hashcode() const
        {
            std::size_t h1 = std::hash<unsigned int>()(value_);
            std::size_t h2 = std::hash<unsigned int>()(format_);
            return h1 ^ (h2 << 1);
        }

        unsigned long sql_time::to_ulong() const
        {
            return value_;
        }
        long long sql_time::to_llong() const
        {
            return value_;
        }
        unsigned long long sql_time::to_ullong() const
        {
            return value_;
        }
        bool sql_time::to_bool() const
        {
            return value_ > 0;
        }

        sql_time::formats sql_time::format() const
        {
            return format_;
        }

        struct tm *sql_time::to_gmtime() const
        {
            return gmtime(&value_);
        }

        struct tm *sql_time::to_localtime() const
        {
            return localtime(&value_);
        }
        string sql_time::to_string() const
        {
            char buf[500] = {0};

            switch (format_) {
                case DATE:
                    strftime(buf, sizeof(buf), "%Y-%m-%d", gmtime(&value_));
                    return buf;
                case TIME:
                    strftime(buf, sizeof(buf), "%H:%M:%S", gmtime(&value_));
                    return buf;
                case TIMESTAMP:
                case DATETIME:
                    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", gmtime(&value_));
                    return buf;
            }
        }

        wstring sql_time::to_wstring() const
        {
            wchar_t buf[500] = {0};

            switch (format_) {
                case DATE:
                    wcsftime(buf, sizeof(buf), L"%Y-%m-%d", gmtime(&value_));
                    return buf;
                case TIME:
                    wcsftime(buf, sizeof(buf), L"%H:%M:%S", gmtime(&value_));
                    return buf;
                case TIMESTAMP:
                case DATETIME:
                    wcsftime(buf, sizeof(buf), L"%Y-%m-%d %H:%M:%S", gmtime(&value_));
                    return buf;
            }
        }

        sql_value::sql_value()
        {
        }

        sql_value::sql_value(const sql_time &value) : variant(std::shared_ptr<sql_time>(new sql_time(value)))
        {
        }

        sql_time sql_value::to_time() const
        {
            auto ptr = dynamic_pointer_cast<sql_time>(to_complex());

            if (ptr) {
                return *ptr;
            }

            return sql_time();
        }

        sql_value::operator sql_time() const
        {
            return to_time();
        }

        bool sql_value::is_time() const
        {
            return is_complex() && dynamic_pointer_cast<sql_time>(to_complex());
        }

        std::string to_string(const sql_null_type &null)
        {
            return "NULL";
        }

        ostream &operator<<(ostream &out, const sql_null_type &null)
        {
            out << "NULL";
            return out;
        }

        ostream &operator<<(ostream &out, const sql_time &value)
        {
            out << value.to_string();
            return out;
        }

        ostream &operator<<(ostream &out, const sql_value &value)
        {
            out << value.to_string();
            return out;
        }
    }
}

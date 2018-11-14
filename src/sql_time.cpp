#include "sql_time.h"
#include "exception.h"
#include "sql_number.h"
#include <cstring>

namespace coda {
    namespace db {
        bool sql_time::parse(const std::string &value) {
            struct tm tp;

            if (value.length() == 0) {
                return false;
            }

            memset(&tp, 0, sizeof(tp));

            if (strptime(value.c_str(), "%Y-%m-%d %H:%M:%S", &tp)) {
                value_ = timegm(&tp);
                format_ = DATETIME;
                return true;
            }

            if (strptime(value.c_str(), "%Y-%m-%d", &tp)) {
                value_ = timegm(&tp);
                format_ = DATE;
                return true;
            }

            if (strptime(value.c_str(), "%H:%M:%S", &tp)) {
                /* set date to 1970-1-1 */
                tp.tm_year = 70;
                tp.tm_mon = 0;
                tp.tm_mday = 1;
                value_ = timegm(&tp);
                format_ = TIME;
                return true;
            }

            try {
                value_ = std::stoul(value);
                format_ = TIMESTAMP;
                return true;
            } catch (...) {
                return false;
            }
        }

        sql_time::sql_time(time_t value, formats format) : value_(value), format_(format) {
        }

        sql_time::sql_time(const std::string &value) : value_(0), format_(TIMESTAMP) {
            if (!parse(value)) {
                throw value_conversion_error("unable to convert string to time");
            }
        }

        sql_time::sql_time(const sql_time &other) : value_(other.value_), format_(other.format_) {
        }

        sql_time::sql_time(sql_time &&other) : value_(other.value_), format_(other.format_) {
        }

        sql_time::~sql_time() {
        }

        sql_time &sql_time::operator=(const sql_time &other) {
            value_ = other.value_;
            format_ = other.format_;
            return *this;
        }

        sql_time &sql_time::operator=(sql_time &&other) {
            value_ = other.value_;
            format_ = other.format_;
            return *this;
        }

        sql_time::formats sql_time::format() const {
            return format_;
        }

        time_t sql_time::value() const {
            return value_;
        }

        struct tm *sql_time::to_gmtime() const {
            return gmtime(&value_);
        }

        struct tm *sql_time::to_localtime() const {
            return localtime(&value_);
        }

        sql_time::operator std::string() const {
            return to_string();
        }

        sql_time::operator std::wstring() const {
            return to_wstring();
        }

        std::string sql_time::to_string() const {
            char buf[500] = {0};

            switch (format_) {
                case sql_time::DATE:
                    strftime(buf, sizeof(buf), "%Y-%m-%d", gmtime(&value_));
                    return buf;
                case sql_time::TIME:
                    strftime(buf, sizeof(buf), "%H:%M:%S", gmtime(&value_));
                    return buf;
                default:
                case sql_time::TIMESTAMP:
                case sql_time::DATETIME:
                    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", gmtime(&value_));
                    return buf;
            }
        }

        std::wstring sql_time::to_wstring() const {
            wchar_t buf[500] = {0};

            switch (format_) {
                case sql_time::DATE:
                    wcsftime(buf, sizeof(buf), L"%Y-%m-%d", gmtime(&value_));
                    return buf;
                case sql_time::TIME:
                    wcsftime(buf, sizeof(buf), L"%H:%M:%S", gmtime(&value_));
                    return buf;
                default:
                case sql_time::TIMESTAMP:
                case sql_time::DATETIME:
                    wcsftime(buf, sizeof(buf), L"%Y-%m-%d %H:%M:%S", gmtime(&value_));
                    return buf;
            }
        }

        bool sql_time::operator==(const sql_time &other) const {
            return value_ == other.value_ && format_ == other.format_;
        }

        sql_time::operator time_t() const {
            return value_;
        }

        std::ostream &operator<<(std::ostream &out, const sql_time &value) {
            out << value.to_string();
            return out;
        }
    }
}

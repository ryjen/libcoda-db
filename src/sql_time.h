#ifndef RJ_DB_SQL_TIME_H
#define RJ_DB_SQL_TIME_H

#include <ctime>
#include <string>

namespace rj
{
    namespace db
    {
        /*!
         * a date/time column format
         */
        class sql_time
        {
           public:
            /*!
             * types of date time values
             */
            typedef enum { DATE, TIME, TIMESTAMP, DATETIME } formats;

            /*!
             * @param value the unix timestamp
             * @param format the format to display
             */
            sql_time(time_t value = time(0), formats format = TIMESTAMP);
            sql_time(const std::string &value);

            sql_time(const sql_time &other);
            sql_time(sql_time &&other);
            sql_time &operator=(const sql_time &other);
            sql_time &operator=(sql_time &&other);
            virtual ~sql_time();

            /*!
             * @return the format of the timestamp
             */
            formats format() const;

            time_t value() const;

            operator time_t() const;
            operator std::string() const;
            operator std::wstring() const;

            /*!
             * @return a time structure based on the timestamp
             */
            struct tm *to_gmtime() const;
            struct tm *to_localtime() const;

            std::string to_string() const;
            std::wstring to_wstring() const;

            bool operator==(const sql_time &other) const;

           private:
            bool parse(const std::string &value);
            time_t value_;
            formats format_;
        };
        std::ostream &operator<<(std::ostream &out, const sql_time &value);
    }
}

#endif

/*!
 * @file sql_value.h
 * a value in a column
 */
#ifndef ARG3_DB_SQL_VALUE_H
#define ARG3_DB_SQL_VALUE_H

#include <cmath>
#include <iostream>
#include "variant.h"

namespace arg3
{
    namespace db
    {
        /*
         * a sql null type
         */
        typedef std::nullptr_t sql_null_type;

        /*!
         * the instance of a null type
         * use this to check for sql null values
         */
        extern const sql_null_type sql_null;

        /*!
         * A sql blob type
         */
        typedef arg3::binary sql_blob;

        /*!
         * a date/time column format
         */
        class sql_time : public variant::complex
        {
            friend class sql_value;

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

            sql_time(const sql_time &other);
            sql_time(sql_time &&other);
            sql_time &operator=(const sql_time &other);
            sql_time &operator=(sql_time &&other);
            virtual ~sql_time();

            /*!
             * @return the size of this custom field
             */
            size_t size() const;

            /*!
             * @return the timestamp as an unsigned int
             */
            unsigned to_uint() const;

            /*!
             * @return the timestamp as an unsigned long
             */
            unsigned long to_ulong() const;

            /*!
             * @return the timestamp as a long long
             */
            long long to_llong() const;

            /*!
             * @return the timestamp as an unsigned long long
             */
            unsigned long long to_ullong() const;

            /*!
             * @return true if the timestamp is greater than zero
             */
            bool to_bool() const;

            /*!
             * Creates a string based on the timestamp format
             * @return the time format string
             */
            std::string to_string() const;

            /*!
             * creates a wide string based on the timestamp format
             * @return the time format wide string
             */
            std::wstring to_wstring() const;

            /*!
             * @return the format of the timestamp
             */
            formats format() const;

            /*!
             * @return a time structure based on the timestamp
             */
            struct tm *to_gmtime() const;

            struct tm *to_localtime() const;

            /*!
             * gets the hashcode for equality
             * @return the hashcode value
             */
            long hashcode() const;

           private:
            time_t value_;
            formats format_;
        };

        /*!
         * A sql value
         */
        class sql_value : public arg3::variant
        {
           public:
            using arg3::variant::variant;

            virtual ~sql_value();

            /*!
             * @param value the sql time value
             */
            sql_value(const sql_time &value);

            /*!
             * default constructor
             */
            sql_value();

            operator sql_time() const;

            /*!
             * @return the sql_time
             */
            sql_time to_time() const;

            /*!
             * @return true if this instance is a sql_time
             */
            bool is_time() const;
        };

        /*!
         * get a string representation of a sql null
         * @param  null the null type
         * @return      a NULL sql string
         */
        std::string to_string(const sql_null_type &null);

        /*!
         * output stream operator for a sql null type
         * will append "NULL"
         * @param out  the output stream
         * @param null  the null type to append
         */
        std::ostream &operator<<(std::ostream &out, const sql_null_type &null);

        std::ostream &operator<<(std::ostream &out, const sql_time &value);

        std::ostream &operator<<(std::ostream &out, const sql_value &value);

        bool operator==(const sql_null_type &null, const sql_value &value);
    }
}

#endif

/*!
 * @file sql_value.h
 * a value in a column
 */
#ifndef ARG3_DB_SQL_VALUE_H
#define ARG3_DB_SQL_VALUE_H

#include "variant.h"
#include <iostream>
#include <cmath>

namespace arg3
{
    namespace db
    {
        class sqldb;
        class base_query;
        class bindable;

        /*
         * a sql null type
         */
        typedef nullptr_t sql_null_type;

        /*!
         * the instance of a null type
         * use this to check for sql null values
         */
        extern const nullptr_t sql_null;

        /*!
         * A sql blob type
         */
        typedef arg3::binary sql_blob;

        /*!
         * a custom variant type
         */
        class sql_custom : public arg3::variant::custom
        {
           public:
        };

        /*!
         * a date/time column format
         */
        class sql_time : public sql_custom
        {
            friend class sql_value;

           private:
            sql_time();

           public:
            /*!
             * types of date time values
             */
            typedef enum { DATE, TIME, DATETIME, TIMESTAMP } formats;

            /*!
             * @param value the unix timestamp
             * @param format the format to display
             */
            sql_time(time_t value, formats format);

            /*!
             * @return the size of this custom field
             */
            size_t size() const;

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
            string to_string() const;

            /*!
             * creates a wide string based on the timestamp format
             * @return the time format wide string
             */
            wstring to_wstring() const;

            /*!
             * @return the format of the timestamp
             */
            formats format() const;

            /*!
             * @return a time structure based on the timestamp
             */
            struct tm *to_gmtime() const;

            /*!
             * tests if this instance is equal with another custom type
             * @return true if the instances are equal
             */
            bool equals(const custom *other) const;

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

            /*!
             * @param value the sql time value
             */
            sql_value(const sql_time &value);

            /*!
             * default constructor
             */
            sql_value();

            /*!
             * @return the sql_time
             */
            sql_time to_time() const;

            /*!
             * @return true if this instance is a sql_time
             */
            bool is_time() const;
        };
    }
}

namespace std
{
    /*
     * some standard to_string functions
     */
    std::string to_string(const arg3::db::sql_null_type &value);
    std::string to_string(const std::string &value);  // yep

    /*!
     * output stream operator for a sql null type
     * will append "NULL"
     * @param out  the output stream
     * @param null  the null type to append
     */
    std::ostream &operator<<(std::ostream &out, const arg3::db::sql_null_type &null);
}


#endif

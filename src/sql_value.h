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

        class sql_custom : public arg3::variant::custom
        {
           public:
        };

        class sql_time : public sql_custom
        {
            friend class sql_value;

           private:
            sql_time();

           public:
            typedef enum { DATE, TIME, DATETIME, TIMESTAMP } formats;

            sql_time(time_t value, formats format);
            size_t size() const;
            unsigned long to_ulong() const;
            long long to_llong() const;
            unsigned long long to_ullong() const;
            bool to_bool() const;
            string to_string() const;
            wstring to_wstring() const;

            formats format() const;
            struct tm *to_gmtime() const;

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

            sql_value(const sql_time &value);

            sql_value();

            sql_time to_time() const;
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

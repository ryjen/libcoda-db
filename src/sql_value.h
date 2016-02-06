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
         * A sql value
         */
        class sql_value : public arg3::variant
        {
           public:
            using arg3::variant::variant;

            /*!
             * binds a sql value to a bindable object
             * @param obj   the object to bind to
             * @param index the parameter index to bind
             * @param value the sql value to bind
             */
            void bind_to(bindable *obj, int index) const;
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

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

        /*!
         * the instance of a null type
         * use this to check for sql null values
         */
        extern const nullptr_t sql_null;

        typedef arg3::binary sql_blob;

        typedef nullptr_t sql_null_t;

        /*!
         * a class to store and convert between the fundamental sql data types
         */
        class sql_value : public arg3::variant
        {
           public:
            using variant::variant;

            void bind_to(bindable *obj, int index) const;
        };
    }
}


namespace std
{
    /*
     * some standard to_string functions
     */

    string to_string(const nullptr_t &value);
    string to_string(const std::string &value);  // yep

    ostream &operator<<(ostream &out, const nullptr_t &null);
}


#endif

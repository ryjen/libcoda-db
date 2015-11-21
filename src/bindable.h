#ifndef ARG3_DB_BINDABLE_H
#define ARG3_DB_BINDABLE_H

#include "sql_value.h"

namespace arg3
{
    namespace db
    {
        /*!
         * represents something that can have a sql value binded to it
         */
        class bindable
        {
           public:
            virtual bindable &bind(size_t index, int value) = 0;
            virtual bindable &bind(size_t index, long long value) = 0;
            virtual bindable &bind(size_t index, double value) = 0;
            virtual bindable &bind(size_t index, const std::string &value, int len = -1) = 0;
            virtual bindable &bind(size_t index, const sql_blob &value) = 0;
            virtual bindable &bind(size_t index, const sql_null_type &value) = 0;
            virtual bindable &bind(size_t index, const void *data, size_t size, void (*pFree)(void *)) = 0;
            virtual bindable &bind_value(size_t index, const sql_value &v) = 0;
        };
    }
}

#endif

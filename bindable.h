#ifndef ARG3_DB_BINDABLE_H_
#define ARG3_DB_BINDABLE_H_

#include "sql_value.h"

namespace arg3
{
    namespace db
    {
        class bindable
        {
        public:
            virtual bindable &bind(size_t index, int value) = 0;
            virtual bindable &bind(size_t index, int64_t value) = 0;
            virtual bindable &bind(size_t index, double value) = 0;
            virtual bindable &bind(size_t index, const std::string &value, int len = -1) = 0;
            virtual bindable &bind(size_t index, const sql_blob &value) = 0;
            virtual bindable &bind(size_t index, const sql_null_t &value) = 0;
            virtual bindable &bind(size_t index, const void *data, size_t size, void(* pFree)(void *) = SQLITE_STATIC) = 0;
            virtual bindable &bind_value(size_t index, const sql_value &v) = 0;
        };

    }
}

#endif

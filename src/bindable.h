/*!
 * @file bindable.h
 * An interface for data binding
 */
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
            template <typename T>
            bindable &bind(size_t index, const T &value)
            {
                bind(index, value);
                return *this;
            }

            template <typename T, typename... Args>
            bindable &bind(size_t index, const T &value, const Args &... argv)
            {
                bind(index, value);
                bind(index, argv...);
                return *this;
            }

            /*!
             * binds an integer value
             * @param  index the index of the binding
             * @param  value the value to bind
             * @return       a reference to this instance
             */
            virtual bindable &bind(size_t index, int value) = 0;

            /*!
             * binds a long long value
             * @param  index the index of the binding
             * @param  value the value to bind
             * @return       a reference to this instance
             */
            virtual bindable &bind(size_t index, long long value) = 0;

            /*!
             * binds a floating point value
             * @param  index the index of the binding
             * @param  value the value to bind
             * @return       a reference to this instance
             */
            virtual bindable &bind(size_t index, double value) = 0;

            /*!
             * binds a string value
             * @param  index the index of the binding
             * @param  value the value to bind
             * @param  len   the length of the binding (default: -1)
             * @return       a reference to this instance
             */
            virtual bindable &bind(size_t index, const std::string &value, int len = -1) = 0;

            /*!
             * binds a wide string value
             * @param  index the index of the binding
             * @param  value the value to bind
             * @param  len   the length of the binding (default: -1)
             * @return       a reference to this instance
             */
            virtual bindable &bind(size_t index, const std::wstring &value, int len = -1) = 0;

            /*!
             * binds a blob value
             * @param  index the index of the binding
             * @param  value the value to bind
             * @return       a reference to this instance
             */
            virtual bindable &bind(size_t index, const sql_blob &value) = 0;

            /*!
             * binds a null value
             * @param  index the index of the binding
             * @param  value the null value to bind
             * @return       a reference to this instance
             */
            virtual bindable &bind(size_t index, const sql_null_type &value) = 0;
        };
    }
}

#endif

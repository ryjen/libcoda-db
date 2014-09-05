/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_COLUMN_VALUE_H_
#define _ARG3_DB_COLUMN_VALUE_H_

#include <sqlite3.h>
#include <string>
#include <cassert>
#include <memory>
#include "exception.h"
#include "sql_value.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        /*!
         * an interface for a database specific implementation of a column
         */
        class column_impl
        {
        public:

            column_impl() = default;
            column_impl(const column_impl &other) = default;
            column_impl(column_impl &&other) = default;
            column_impl &operator=(const column_impl &other) = default;
            column_impl &operator=(column_impl && other) = default;
            virtual ~column_impl() = default;

            virtual bool is_valid() const = 0;

            virtual sql_blob to_blob() const = 0;

            virtual double to_double() const = 0;

            virtual int to_int() const = 0;

            virtual bool to_bool() const = 0;

            virtual int64_t to_int64() const = 0;

            virtual string to_string() const = 0;

            virtual sql_value to_value() const = 0;

            virtual string name() const = 0;

        };

        /*!
         * a column holds a value for a row
         */
        class column
        {
            template<class A, class B, class C> friend class row_iterator;
        private:
            shared_ptr<column_impl> impl_;
            column();
        public:
            column(shared_ptr<column_impl> impl);

            virtual ~column() = default;

            column(const column &other);

            column(column &&other);

            column &operator=(const column &other);

            column &operator=(column && other);

            bool is_valid() const;

            sql_blob to_blob() const;

            double to_double() const;

            int to_int() const;

            bool to_bool() const;

            int64_t to_int64() const;

            string to_string() const;

            sql_value to_value() const;

            operator string() const;

            operator int() const;

            operator int64_t() const;

            operator double() const;

            string name() const;

            shared_ptr<column_impl> impl() const;
        };

    }
}

#endif

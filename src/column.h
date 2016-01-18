/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef ARG3_DB_COLUMN_VALUE_H
#define ARG3_DB_COLUMN_VALUE_H

#include <string>
#include <cassert>
#include <memory>
#include "exception.h"
#include "sql_value.h"

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
            column_impl &operator=(column_impl &&other) = default;
            virtual ~column_impl() = default;

            virtual bool is_valid() const = 0;

            virtual sql_value to_value() const = 0;

            virtual std::string name() const = 0;
        };

        /*!
         * a column holds a value for a row
         */
        class column
        {
            template <class A, class B, class C>
            friend class row_iterator;

           private:
            std::shared_ptr<column_impl> impl_;
            column();

           public:
            column(std::shared_ptr<column_impl> impl);

            virtual ~column() = default;

            column(const column &other);

            column(column &&other);

            column &operator=(const column &other);

            column &operator=(column &&other);

            bool is_valid() const;

            sql_value to_value() const;

            std::string name() const;

            std::shared_ptr<column_impl> impl() const;
        };
    }
}

#endif

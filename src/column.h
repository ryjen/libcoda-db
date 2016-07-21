/*!
 * @file column.h
 * represents a column in a row in a database
 * @copyright ryan jennings (ryan-jennings.net), 2013
 */
#ifndef RJ_DB_COLUMN_VALUE_H
#define RJ_DB_COLUMN_VALUE_H

#include <cassert>
#include <memory>
#include <string>
#include "exception.h"
#include "sql_value.h"

namespace rj
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

            /*!
             * tests if this colums data validity
             * @return true if this columns data is valid
             */
            virtual bool is_valid() const = 0;

            /*!
             * converts this column to a value
             * @return the value of this column
             */
            virtual sql_value to_value() const = 0;

            /*!
             * @return the name of this column;
             */
            virtual std::string name() const = 0;
        };

        /*!
         * a column holds a value
         */
        class column
        {
            template <class A, class B, class C>
            friend class row_iterator;

           private:
            std::shared_ptr<column_impl> impl_;
            column();

           public:
            /*!
             * default constructor requires an implementation
             * @param impl the database specific implementation
             */
            column(const std::shared_ptr<column_impl> &impl);

            virtual ~column() = default;

            /*! copy constructor */
            column(const column &other);

            /*! move constructor */
            column(column &&other);

            /*! copy assignment */
            column &operator=(const column &other);

            /*! move assignment */
            column &operator=(column &&other);

            /*!
             * tests if this column has valid data
             * @return true if this columns data is valid
             */
            bool is_valid() const;

            /*!
             * converts this column to a value
             * @return the value of this column
             */
            sql_value to_value() const;

            /*!
             * converts this column to a blob value_
             */
            sql_blob to_blob() const;

            /*!
             * converts this column to a sql time value
             */
            sql_time to_time() const;

            /*!
             * @return the name of this column
             */
            std::string name() const;

            /*!
             * @return the instance of the implementation
             */
            std::shared_ptr<column_impl> impl() const;

            /*!
             * primitive operators
             */
            operator std::string() const;
            operator unsigned() const;
            operator double() const;
            operator std::wstring() const;
            operator int() const;
            operator long long() const;
            operator unsigned long long() const;
            operator float() const;
        };
    }
}

#endif

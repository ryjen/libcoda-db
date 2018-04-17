/*!
 * @file column.h
 * represents a column in a row in a database
 * @copyright ryan jennings (ryan-jennings.net), 2013
 */
#ifndef CODA_DB_COLUMN_VALUE_H
#define CODA_DB_COLUMN_VALUE_H

#include <cassert>
#include <memory>
#include <string>
#include "sql_types.h"

namespace coda {
    namespace db {
        class sql_value;

        /*!
         * an interface for a database specific implementation of a column
         */
        class column_impl {
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
        class column : public sql_value_convertible {
            template<class A, class B, class C>
            friend
            class row_iterator;

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
            sql_value value() const;

            /*!
             * @return the name of this column
             */
            std::string name() const;

            /*!
             * @return the instance of the implementation
             */
            std::shared_ptr<column_impl> impl() const;

            operator sql_string() const;

            operator sql_wstring() const;

            operator sql_number() const;

            operator sql_time() const;

            operator sql_blob() const;

            operator bool() const;

            operator char() const;

            operator unsigned char() const;

            operator wchar_t() const;

            operator short() const;

            operator unsigned short() const;

            operator int() const;

            operator unsigned int() const;

            operator long() const;

            operator unsigned long() const;

            operator long long() const;

            operator unsigned long long() const;

            operator float() const;

            operator double() const;

            operator long double() const;

            bool operator==(const sql_null_type &other) const;

            bool operator==(const sql_number &value) const;

            bool operator==(const sql_string &value) const;

            bool operator==(const sql_wstring &value) const;

            bool operator==(const sql_time &value) const;

            bool operator==(const sql_blob &value) const;

            bool operator==(const bool &value) const;

            bool operator==(const char &value) const;

            bool operator==(const unsigned char &value) const;

            bool operator==(const wchar_t &value) const;

            bool operator==(const short &value) const;

            bool operator==(const unsigned short &value) const;

            bool operator==(const int &value) const;

            bool operator==(const unsigned int &value) const;

            bool operator==(const long &value) const;

            bool operator==(const unsigned long &value) const;

            bool operator==(const long long &value) const;

            bool operator==(const unsigned long long &value) const;

            bool operator==(const float &value) const;

            bool operator==(const double &value) const;

            bool operator==(const long double &value) const;

            bool operator==(const sql_value &other) const;
        };

        bool operator==(const sql_value &value, const column &column);
    }
}

#endif

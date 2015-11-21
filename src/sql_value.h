#ifndef ARG3_DB_SQL_VALUE_H
#define ARG3_DB_SQL_VALUE_H

#include "variant.h"
#include <iostream>
#include <cmath>

#define DOUBLE_DEFAULT 0.0
#define INT_DEFAULT 0
#define BOOL_DEFAULT false
#define VALUE_DEFAULT NULL;

namespace arg3
{
    namespace db
    {
        class sqldb;
        class base_query;
        class bindable;

        /*!
         * a wrapper for a blob sql type
         */
        class sql_blob
        {
           public:
            friend std::ostream &operator<<(std::ostream &out, const sql_blob &value);
            typedef void (*cleanup_method)(void *);

           private:
            const void *p_;
            size_t s_;
            cleanup_method destruct_;
            void copy(const sql_blob &other);
            void clear();

           public:
            sql_blob(const void *ptr, size_t size, cleanup_method cleanup);
            sql_blob(const void *ptr, size_t size);
            sql_blob(const sql_blob &other);
            sql_blob(sql_blob &&other);
            sql_blob &operator=(const sql_blob &other);
            sql_blob &operator=(sql_blob &&other);
            virtual ~sql_blob();
            const void *ptr() const;
            size_t size() const;
            cleanup_method destructor() const;
            std::string to_string() const;
        };

        /*!
         * a quick and dirty representation of a sql null type
         */
        class sql_null_type
        {
           public:
            static const sql_null_type instance;

           private:
            sql_null_type()
            {
            }
        };

        /*!
         * the instance of a null type
         * use this to check for sql null values
         */
        extern const sql_null_type sql_null;

        /*!
         * a class to store and convert between the fundamental sql data types
         */
        class sql_value
        {
            friend std::ostream &operator<<(std::ostream &out, const sql_value &value);
            friend bool operator==(const std::string &other, const sql_value &value);
            friend bool operator==(int other, const sql_value &value);
            friend bool operator==(long long other, const sql_value &value);
            friend bool operator==(double other, const sql_value &value);
            friend bool operator==(const sql_null_type &other, const sql_value &value);

           private:
            // nifty variable template class
            arg3::variant value_;

           public:
            sql_value();

            sql_value(const sql_value &other);

            sql_value(sql_value &&other);

            sql_value &operator=(const sql_value &other);

            sql_value &operator=(sql_value &&other);

            ~sql_value();

            sql_value(const std::string &value) : value_(value)
            {
            }

            sql_value(const char *value) : value_(value)
            {
            }

            sql_value(int value) : value_(value)
            {
            }

            sql_value(long long value) : value_(value)
            {
            }

            sql_value(double value) : value_(value)
            {
            }

            sql_value(const sql_null_type &value) : value_(nullptr)
            {
            }

            sql_value(const sql_blob &value) : value_(value.ptr(), value.size())
            {
            }

            operator std::string() const;

            operator int() const;

            operator long long() const;

            operator double() const;

            operator sql_blob() const;

            operator bool() const;

            std::string to_string() const;

            bool is_valid() const;

            int to_int(const int def = INT_DEFAULT) const;

            long long to_llong(const long long def = INT_DEFAULT) const;

            double to_double(const double def = DOUBLE_DEFAULT) const;

            bool to_bool(const bool def = BOOL_DEFAULT) const;

            sql_blob to_blob() const;

            void bind_to(bindable *obj, int index) const;

            bool operator==(const sql_value &other) const;

            bool operator==(const sql_null_type &other) const;

            bool operator==(const int &other) const;

            bool operator==(const long long &other) const;

            bool operator==(const double &other) const;

            bool operator==(const std::string &other) const;

            bool operator!=(const sql_value &other) const;

            bool operator!=(const sql_null_type &other) const;

            bool operator!=(const int &other) const;

            bool operator!=(const long long &other) const;

            bool operator!=(const double &other) const;

            bool operator!=(const std::string &other) const;
        };

        std::ostream &operator<<(std::ostream &out, const sql_value &value);

        std::ostream &operator<<(std::ostream &out, const sql_null_type &value);

        std::ostream &operator<<(std::ostream &out, const sql_blob &value);

        inline bool operator==(const sql_null_type &a, const sql_null_type &b)
        {
            return true;
        }

        inline bool operator==(const std::string &other, const sql_value &value)
        {
            return other == value.value_;
        }

        inline bool operator==(int other, const sql_value &value)
        {
            return other == value.value_;
        }

        inline bool operator==(long long other, const sql_value &value)
        {
            return other == value.value_;
        }

        inline bool operator==(double other, const sql_value &value)
        {
            return other == value.value_;
        }

        inline bool operator==(const sql_null_type &other, const sql_value &value)
        {
            return value.value_.is_null();
        }

        inline bool operator!=(const std::string &other, const sql_value &value)
        {
            return !operator==(other, value);
        }

        inline bool operator!=(int other, const sql_value &value)
        {
            return !operator==(other, value);
        }

        inline bool operator!=(long long other, const sql_value &value)
        {
            return !operator==(other, value);
        }

        inline bool operator!=(double other, const sql_value &value)
        {
            return !operator==(other, value);
        }

        inline bool operator!=(const sql_null_type &other, const sql_value &value)
        {
            return !operator==(other, value);
        }
    }
}


namespace std
{
    /*
     * some standard to_string functions
     */

    string to_string(const arg3::db::sql_null_type &value);
    string to_string(const arg3::db::sql_blob &value);
    string to_string(const std::string &value);  // yep
    string to_string(const arg3::db::sql_value &value);
}


#endif

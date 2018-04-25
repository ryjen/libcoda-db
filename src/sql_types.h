#ifndef CODA_DB_SQL_TYPES_H
#define CODA_DB_SQL_TYPES_H

#include <string>
#include <vector>
#include <memory>

namespace coda {
    namespace db {
        /*
          * a sql null type
          */
        typedef std::nullptr_t sql_null_type;

        /*!
         * the instance of a null type
         * use this to check for sql null values
         */
        extern const sql_null_type sql_null;

        typedef std::string sql_string;
        typedef std::wstring sql_wstring;

        class sql_number;

        class sql_time;

        /*!
         * blobs are simple vectors of unsigned chars
         */
        class sql_blob {
        public:
            sql_blob();
            sql_blob(const void *data, size_t size);
            void *get() const;
            size_t size() const;
        private:
            std::shared_ptr<void> value_;
            size_t size_;
        };

        /**
         * template helper to see if a type is appropriate for a sql number
         */
        template<typename T>
        struct is_sql_number
                : std::integral_constant<bool, std::is_arithmetic<T>::value || std::is_same<T, sql_null_type>::value> {
        };

        template<typename T>
        struct is_sql_string
                : std::integral_constant<bool,
                        std::is_same<T, sql_string>::value || std::is_same<T, sql_wstring>::value> {
        };

        /*!
         * defines a constant if a type is a valid sql value
         */
        template<typename T>
        struct is_sql_value
                : std::integral_constant<bool,
                        std::is_same<T, sql_null_type>::value || std::is_same<T, sql_number>::value ||
                        std::is_same<T, sql_string>::value || std::is_same<T, sql_wstring>::value ||
                        std::is_same<T, sql_time>::value || std::is_same<T, sql_blob>::value> {
        };

        /*!
         * an interface for converting and testing equality of numeric values
         */
        class sql_number_convertible {
        public:
            sql_number_convertible() = default;

            sql_number_convertible(const sql_number_convertible &other) = default;

            sql_number_convertible(sql_number_convertible &&other) = default;

            virtual ~sql_number_convertible() = default;

            sql_number_convertible &operator=(const sql_number_convertible &other) = default;

            sql_number_convertible &operator=(sql_number_convertible &&other) = default;

            /* convertibles */
            virtual operator sql_string() const = 0;

            virtual operator sql_wstring() const = 0;

            virtual operator sql_time() const = 0;

            /* numeric implicits */
            virtual operator bool() const = 0;

            virtual operator char() const = 0;

            virtual operator unsigned char() const = 0;

            virtual operator wchar_t() const = 0;

            virtual operator short() const = 0;

            virtual operator unsigned short() const = 0;

            virtual operator int() const = 0;

            virtual operator unsigned int() const = 0;

            virtual operator long() const = 0;

            virtual operator unsigned long() const = 0;

            virtual operator long long() const = 0;

            virtual operator unsigned long long() const = 0;

            virtual operator float() const = 0;

            virtual operator double() const = 0;

            virtual operator long double() const = 0;

            /* convertible equality */
            virtual bool operator==(const sql_null_type &other) const = 0;

            virtual bool operator==(const sql_string &value) const = 0;

            virtual bool operator==(const sql_wstring &value) const = 0;

            virtual bool operator==(const sql_time &value) const = 0;

            /* numeric equality */
            virtual bool operator==(const bool &value) const = 0;

            virtual bool operator==(const char &value) const = 0;

            virtual bool operator==(const unsigned char &value) const = 0;

            virtual bool operator==(const wchar_t &value) const = 0;

            virtual bool operator==(const short &value) const = 0;

            virtual bool operator==(const unsigned short &value) const = 0;

            virtual bool operator==(const int &value) const = 0;

            virtual bool operator==(const unsigned int &value) const = 0;

            virtual bool operator==(const long &value) const = 0;

            virtual bool operator==(const unsigned long &value) const = 0;

            virtual bool operator==(const long long &value) const = 0;

            virtual bool operator==(const unsigned long long &value) const = 0;

            virtual bool operator==(const float &value) const = 0;

            virtual bool operator==(const double &value) const = 0;

            virtual bool operator==(const long double &value) const = 0;
        };

        /*!
         * interface for converting and testing equality of sql values
         */
        class sql_value_convertible : public sql_number_convertible {
        public:
            virtual operator sql_number() const = 0;

            virtual operator sql_blob() const = 0;

            virtual bool operator==(const sql_number &value) const = 0;

            virtual bool operator==(const sql_blob &value) const = 0;
        };
    }
}

#endif

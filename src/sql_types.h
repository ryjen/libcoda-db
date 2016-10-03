#ifndef RJ_DB_SQL_TYPES_H
#define RJ_DB_SQL_TYPES_H

#include <string>
#include <vector>

namespace rj
{
    namespace db
    {
        /*
          * a sql null type
          */
        typedef std::nullptr_t sql_null_type;

        /*!
         * the instance of a null type
         * use this to check for sql null values
         */
        extern const sql_null_type sql_null;

        typedef std::vector<unsigned char> sql_blob;

        typedef std::string sql_string;
        typedef std::wstring sql_wstring;

        class sql_number;
        class sql_time;
        class sql_value;

        /**
         * template helper to see if a type is appropriate for a sql number
         */
        template <typename T>
        struct is_sql_number : std::integral_constant<bool, std::is_arithmetic<T>::value || std::is_same<T, sql_null_type>::value> {
        };

        template <typename T>
        struct is_sql_value : std::integral_constant<bool, std::is_same<T, sql_null_type>::value || std::is_same<T, sql_number>::value ||
                                                               std::is_same<T, sql_string>::value || std::is_same<T, sql_wstring>::value ||
                                                               std::is_same<T, sql_time>::value || std::is_same<T, sql_blob>::value> {
        };

        class sql_number_convertible
        {
           public:
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


        class sql_value_convertible : public sql_number_convertible
        {
           public:
            virtual operator sql_number() const = 0;
            virtual operator sql_blob() const = 0;

            virtual bool operator==(const sql_number &value) const = 0;
            virtual bool operator==(const sql_blob &value) const = 0;
        };
    }
}

#endif

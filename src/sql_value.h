/*!
 * @file sql_value.h
 * a value in a column
 */
#ifndef CODA_DB_SQL_VALUE_H
#define CODA_DB_SQL_VALUE_H

#include <type_traits>
#include "sql_number.h"
#include "sql_time.h"

namespace coda::db {
    /*!
     * A sql value
     */
    class sql_value : public sql_value_convertible {
     public:
      sql_value();

      sql_value(const sql_value &other) = default;
      sql_value(sql_value &&other) noexcept = default;

      ~sql_value() override = default;

      sql_value &operator=(const sql_value &other) = default;
      sql_value &operator=(sql_value &&other) noexcept = default;

       sql_value(const sql_null_type &value);

       sql_value(const sql_number &value);

       sql_value(const sql_string &value);

       sql_value(const sql_wstring &value);

       sql_value(const sql_time &value);

       sql_value(const sql_blob &value);

      /**
       * numeric constructors
       * would be nice to use a template here, but it can't deduce the type very
       * well
       */
       sql_value(const bool &value);

       sql_value(const char &value);

       sql_value(const unsigned char &value);

       sql_value(const wchar_t &value);

       sql_value(const short &value);

       sql_value(const unsigned short &value);

       sql_value(const int &value);

       sql_value(const unsigned int &value);

       sql_value(const long &value);

       sql_value(const unsigned long &value);

       sql_value(const long long &value);

       sql_value(const unsigned long long &value);

       sql_value(const float &value);

       sql_value(const double &value);

       sql_value(const long double &value);

       sql_value(const char *value, std::string::size_type len = std::string::npos);

       sql_value(const wchar_t *value, std::wstring::size_type len = std::wstring::npos);

      template <typename T, typename = std::enable_if<is_sql_value<T>::value || is_sql_number<T>::value>>
      constexpr bool is() const noexcept {
        return std::visit(helper::is_type<T>(), value_);
      }

      template <typename T, typename = std::enable_if<is_sql_number<T>::value>>
      constexpr T as() const {
        return std::visit(helper::as_number<T>(), value_);
      }

       operator sql_number() const override;

       operator sql_string() const override;

       operator sql_wstring() const override;

       operator sql_time() const override;

       operator sql_blob() const override;

      /* numeric implicits */
       operator bool() const override;

       operator char() const override;

       operator unsigned char() const override;

       operator wchar_t() const override;

       operator short() const override;

       operator unsigned short() const override;

       operator int() const override;

       operator unsigned int() const override;

       operator long() const override;

       operator unsigned long() const override;

       operator long long() const override;

       operator unsigned long long() const override;

       operator float() const override;

       operator double() const override;

       operator long double() const override;

      bool operator==(const sql_value &other) const;

      bool operator==(const sql_null_type &other) const override;

      bool operator==(const sql_number &value) const override;

      bool operator==(const sql_string &value) const override;

      bool operator==(const sql_wstring &value) const override;

      bool operator==(const sql_time &value) const override;

      bool operator==(const sql_blob &value) const override;

      /* numeric equality */
      bool operator==(const bool &value) const override;

      bool operator==(const char &value) const override;

      bool operator==(const unsigned char &value) const override;

      bool operator==(const wchar_t &value) const override;

      bool operator==(const short &value) const override;

      bool operator==(const unsigned short &value) const override;

      bool operator==(const int &value) const override;

      bool operator==(const unsigned int &value) const override;

      bool operator==(const long &value) const override;

      bool operator==(const unsigned long &value) const override;

      bool operator==(const long long &value) const override;

      bool operator==(const unsigned long long &value) const override;

      bool operator==(const float &value) const override;

      bool operator==(const double &value) const override;

      bool operator==(const long double &value) const override;

      std::string to_string() const;

      std::wstring to_wstring() const;

      template <typename V, typename T>
      T apply_visitor(const V &visitor) const {
        return std::visit(visitor, value_);
      }

      template <typename V>
      void apply_visitor(const V &visitor) const {
        std::visit(visitor, value_);
      }

     private:
      std::variant<sql_null_type, sql_number, sql_string, sql_wstring, sql_time, sql_blob> value_;
    };

    template <>
    sql_string sql_value::as<sql_string>() const;

    template <>
    sql_wstring sql_value::as<sql_wstring>() const;

    template <>
    sql_time sql_value::as<sql_time>() const;

    template <>
    sql_blob sql_value::as<sql_blob>() const;

    template <>
    sql_number sql_value::as<sql_number>() const;

    /*!
     * output stream operator for a sql null type
     * will append "NULL"
     * @param out  the output stream
     * @param null  the null type to append
     */
    std::ostream &operator<<(std::ostream &out, const sql_value &value);

    bool operator==(const sql_null_type &null, const sql_value &value);

    bool operator==(const sql_time &time, const sql_value &value);

    bool operator==(const sql_string &other, const sql_value &value);

    bool operator==(const sql_wstring &other, const sql_value &value);

    bool operator==(const sql_number &other, const sql_value &value);

    bool operator==(const sql_blob &other, const sql_value &value);

    /* numeric equality */
    bool operator==(const bool &value, const sql_value &other);

    bool operator==(const char &value, const sql_value &other);

    bool operator==(const unsigned char &value, const sql_value &other);

    bool operator==(const wchar_t &value, const sql_value &other);

    bool operator==(const short &value, const sql_value &other);

    bool operator==(const unsigned short &value, const sql_value &other);

    bool operator==(const int &value, const sql_value &other);

    bool operator==(const unsigned int &value, const sql_value &other);

    bool operator==(const long &value, const sql_value &other);

    bool operator==(const unsigned long &value, const sql_value &other);

    bool operator==(const long long &value, const sql_value &other);

    bool operator==(const unsigned long long &value, const sql_value &other);

    bool operator==(const float &value, const sql_value &other);

    bool operator==(const double &value, const sql_value &other);

    bool operator==(const long double &value, const sql_value &other);
}  // namespace coda::db

#endif

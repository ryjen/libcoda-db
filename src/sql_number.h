
#ifndef CODA_DB_SQL_NUMBER_H
#define CODA_DB_SQL_NUMBER_H

#include <algorithm>
#include <string>
#include <variant>
#include <vector>
#include "exception.h"
#include "sql_common.h"
#include "sql_time.h"

namespace coda::db {

    namespace helper {
      template <typename T>
      class as_number {
       public:
        template <typename V>
        T operator()(const V &value) const {
          return value;
        }

        T operator()(const sql_null_type &value) const { return 0; }

        T operator()(const sql_blob &value) const { throw value_conversion_error(); }

        T operator()(const sql_time &value) const {
          if (std::is_same<T, time_t>::value || std::is_convertible<time_t, T>::value) {
            return sql_number(value);
          }
          throw value_conversion_error();
        }

        T operator()(const sql_string &value) const { return sql_number(value); }

        T operator()(const sql_wstring &value) const { return sql_number(value); }

        T operator()(const sql_number &value) const { return value; }
      };
    }  // namespace helper
    class sql_number : public sql_number_convertible {
     public:
      sql_number();

       sql_number(const bool &value);

       sql_number(const char &value);

       sql_number(const unsigned char &value);

       sql_number(const wchar_t &value);

       sql_number(const short &value);

       sql_number(const unsigned short &value);

       sql_number(const int &value);

       sql_number(const unsigned int &value);

       sql_number(const long &value);

       sql_number(const unsigned long &value);

       sql_number(const long long &value);

       sql_number(const unsigned long long &value);

       sql_number(const float &value);

       sql_number(const double &value);

       sql_number(const long double &value);

       sql_number(const sql_string &value);

       sql_number(const sql_wstring &value);

       sql_number(const sql_null_type &value);

       sql_number(const sql_time &value);

      sql_number(const sql_number &other) = default;

      sql_number(sql_number &&other) noexcept = default;

      sql_number &operator=(const sql_number &other) = default;

      sql_number &operator=(sql_number &&other) noexcept = default;

      ~sql_number() override = default;

      template <typename T, typename = std::enable_if<is_sql_number<T>::value>>
      bool is() const {
        return std::visit(helper::is_type<T>(), value_);
      }

      template <typename T, typename = std::enable_if<is_sql_number<T>::value>>
      T as() const {
        return std::visit(helper::as_number<T>(), value_);
      }

      /**
       * not a template because it some cases it can't deduce the type
       */
       operator sql_string() const override;

       operator sql_wstring() const override;

       operator sql_time() const override;

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

      std::string to_string() const;

      std::wstring to_wstring() const;

      template <typename S, typename = std::enable_if<is_sql_string<S>::value>>
      bool parse(const S &value) {
        if (!std::any_of(value.begin(), value.end(), ::isdigit)) {
          return parse_bool(value);
        }
        if (value.find('.') != std::string::npos) {
          return parse_floating<float>(value, std::stof) || parse_floating<double>(value, std::stod) ||
                 parse_floating<long double>(value, std::stold);
        }
        return parse_integral<int>(value, std::stoi) || parse_integral<long>(value, std::stol) ||
               parse_integral<unsigned long>(value, std::stoul) || parse_integral<long long>(value, std::stoll) ||
               parse_integral<unsigned long long>(value, std::stoull);
      }

      template <typename V, typename T>
      T apply_visitor(const V &visitor) const {
        return std::visit(visitor, value_);
      }

      template <typename V>
      void apply_visitor(const V &visitor) const {
        std::visit(visitor, value_);
      }

      bool operator==(const sql_number &other) const;

      bool operator==(const sql_null_type &other) const override;

      bool operator==(const sql_string &value) const override;

      bool operator==(const sql_wstring &value) const override;

      bool operator==(const sql_time &value) const override;

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

     private:
      constexpr static const int BASE10 = 10;

      template <typename T, typename S>
      typename std::enable_if<std::is_integral<T>::value && is_sql_string<S>::value, bool>::type parse_integral(
          const S &value, T (*funk)(const S &, size_t *, int), int base = BASE10) {
        try {
          value_ = funk(value, nullptr, base);
          return true;
        } catch (const std::exception &e) {
          return false;
        }
      }

      template <typename T, typename S>
      typename std::enable_if<std::is_floating_point<T>::value && is_sql_string<S>::value, bool>::type parse_floating(
          const S &value, T (*funk)(const S &, size_t *)) {
        try {
          value_ = funk(value, nullptr);
          return true;
        } catch (const std::exception &e) {
          return false;
        }
      }

      template <typename S, typename = std::enable_if<is_sql_string<S>::value>>
      bool parse_bool(const S &value) {
        int test = helper::is_bool(value);
        if (test) {
          value_ = test > 0;
          return true;
        }
        return false;
      }

      std::variant<sql_null_type, bool, char, unsigned char, wchar_t, short, unsigned short, int, unsigned int, long,
                   unsigned long, long long, unsigned long long, float, double, long double> value_;
    };

    template <>
    sql_string sql_number::as<sql_string>() const;

    template <>
    sql_wstring sql_number::as() const;

    template <>
    sql_time sql_number::as() const;

    std::ostream &operator<<(std::ostream &out, const sql_number &value);
}  // namespace coda::db

#endif

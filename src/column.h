/*!
 * @file column.h
 * represents a column in a row in a database
 * @copyright ryan jennings (coda.life), 2013
 */
#ifndef CODA_DB_COLUMN_VALUE_H
#define CODA_DB_COLUMN_VALUE_H

#include <cassert>
#include <memory>
#include <string>
#include "sql_types.h"
#include "sql_value.h"

namespace coda::db {
  class sql_value;

  /*!
   * an interface for a database specific implementation of a column
   */
  class column_impl {
   public:
    column_impl() = default;

    column_impl(const column_impl &other) = default;

    column_impl(column_impl &&other) noexcept = default;

    column_impl &operator=(const column_impl &other) = default;

    column_impl &operator=(column_impl &&other) noexcept = default;

    ~column_impl() = default;

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

    column() = default;

   public:
    /*!
     * default constructor requires an implementation
     * @param impl the database specific implementation
     */
    explicit column(const std::shared_ptr<column_impl> &impl);

    ~column() = default;

    /*! copy constructor */
    column(const column &other);

    /*! move constructor */
    column(column &&other) noexcept;

    /*! copy assignment */
    column &operator=(const column &other);

    /*! move assignment */
    column &operator=(column &&other) noexcept;

    /*!
     * tests if this column has valid data
     * @return true if this columns data is valid
     */
    bool is_valid() const noexcept;

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

    template<typename T>
    constexpr T as() const {
      return value().as<T>();
    }

    operator sql_string() const override;

    operator sql_wstring() const override;

    operator sql_number() const override;

    operator sql_time() const override;

    operator sql_blob() const override;

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

    bool operator==(const sql_null_type &other) const override;

    bool operator==(const sql_number &value) const override;

    bool operator==(const sql_string &value) const override;

    bool operator==(const sql_wstring &value) const override;

    bool operator==(const sql_time &value) const override;

    bool operator==(const sql_blob &value) const override;

    bool operator==(const char *value) const override;

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

    bool operator==(const sql_value &other) const;
  };

  bool operator==(const sql_value &value, const column &column);
}  // namespace coda::db

#endif

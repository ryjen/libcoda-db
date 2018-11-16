/*!
 * @file row.h
 * A row in a result set
 */
#ifndef CODA_DB_ROW_H
#define CODA_DB_ROW_H

#include <functional>
#include <iterator>
#include <memory>
#include "column.h"

namespace coda::db {
    /*!
     * Implementation spefic interface for a row
     */
    class row_impl {
     public:
      typedef coda::db::column column_type;

      row_impl() = default;

      row_impl(const row_impl &other) = default;

      row_impl(row_impl &&other) = default;

      row_impl &operator=(const row_impl &other) = default;

      row_impl &operator=(row_impl &&other) = default;

      virtual ~row_impl() = default;

      /*!
       * gets the name of a column in the row
       * @param  position the index of the column
       * @return          the name of the column at the given position
       */
      virtual std::string column_name(size_t position) const = 0;

      /*!
       * gets a column in the row
       * @param  position the index of the column
       * @return          the column object
       */
      virtual column_type column(size_t position) const = 0;

      /*!
       * gets a column in the row
       * @param  name the name of the column
       * @return      the column object
       */
      virtual column_type column(const std::string &name) const = 0;

      /*!
       * gets the number of columns in the row
       * @return the number of columns;
       */
      virtual size_t size() const noexcept = 0;

      /*!
       * tests if the row internals are valid
       * @return true if the row is valid
       */
      virtual bool is_valid() const noexcept = 0;
    };

    /*!
     * iterator for columns in a row
     */
    template <class ValueType, class NonConst, class RowType>
    class row_iterator : public std::iterator<std::random_access_iterator_tag, ValueType> {
     protected:
      std::shared_ptr<RowType> row_;
      long position_;
      NonConst currentValue_;

      void set_current_value(long index) {
        if (row_ == nullptr) {
          return;
        }

        if (index >= 0 && index < row_->size()) {
          currentValue_ = row_->column(index);
        } else {
          currentValue_ = NonConst();
        }
      }

     public:
      row_iterator() : row_(nullptr), position_(-1) {}

      row_iterator(const std::shared_ptr<RowType> &pRow, long position) : row_(pRow), position_(position) {}

      row_iterator(const row_iterator &other) : row_(other.row_), position_(other.position_) {}

      row_iterator(row_iterator &&other) noexcept : row_(std::move(other.row_)), position_(other.position_) {
        other.row_ = nullptr;
      }

      ~row_iterator() = default;

      row_iterator &operator=(const row_iterator &other) {
        row_ = other.row_;
        position_ = other.position_;
        return *this;
      }

      row_iterator &operator=(row_iterator &&other) noexcept {
        row_ = std::move(other.row_);
        position_ = other.position_;
        other.row_ = nullptr;
        return *this;
      }

      ValueType &operator*() {
        set_current_value(position_);
        return currentValue_;
      }

      ValueType *operator->() {
        set_current_value(position_);
        return &currentValue_;
      }

      ValueType operator[](size_t nPosition) {
        set_current_value(nPosition);
        return currentValue_;
      }

      row_iterator &operator++() {
        if (position_ < row_->size()) {
          ++position_;
        }
        return *this;
      }

      const row_iterator operator++(int) {
        row_iterator tmp(*this);
        ++(*this);
        return tmp;
      }

      row_iterator &operator--() {
        if (position_ > 0) {
          --position_;
        }
        return *this;
      }

      const row_iterator operator--(int) {
        row_iterator tmp(*this);
        --(*this);
        return tmp;
      }

      row_iterator operator+(int n) {
        row_iterator tmp(*this);
        tmp += n;
        return tmp;
      }

      row_iterator &operator+=(int n) {
        position_ = std::min(position_ + n, row_->size());
        return *this;
      }

      row_iterator operator-(int n) {
        row_iterator tmp(*this);
        tmp -= n;
        return tmp;
      }

      row_iterator &operator-=(int n) {
        position_ = std::max(position_ - n, 0L);
        return *this;
      }

      bool operator==(const row_iterator &other) const { return position_ == other.position_; }

      bool operator!=(const row_iterator &other) const { return !operator==(other); }

      bool operator<(const row_iterator &other) const { return position_ < other.position_; }

      bool operator<=(const row_iterator &other) const { return operator<(other) || operator==(other); }

      bool operator>(const row_iterator &other) const { return !operator<(other); }

      bool operator>=(const row_iterator &other) const { return operator>(other) || operator==(other); }

      long operator-(const row_iterator &other) {
        if (position_ >= other.position_)
          return position_ - other.position_;
        else
          return 0;
      }

      std::string name() const {
        if (row_ == nullptr) {
          return nullptr;
        }

        return row_->column_name(position_);
      }
    };

    /*!
     * represent a row in a table
     * implementation is specific to a type of database
     */
    class row {
     private:
      std::shared_ptr<row_impl> impl_;

     public:
      typedef coda::db::column column_type;
      typedef row_iterator<column_type, column_type, row_impl> iterator;
      typedef row_iterator<const column_type, column_type, const row_impl> const_iterator;

      /*!
       * default constructor
       */
      row() = default;

      /*!
       * @param impl  the row implementation
       */
      explicit row(const std::shared_ptr<row_impl> &impl);

      row(const row &other) = default;

      row(row &&other) noexcept = default;

      ~row() = default;

      row &operator=(const row &other) = default;

      row &operator=(row &&other) noexcept = default;

      /*!
       * @return an iterator to the first column
       */
      iterator begin();

      /*!
       * @return an const iterator to the first column
       */
      const_iterator begin() const;

      /*!
       * @return an immutable iterator to the first column
       */
      const_iterator cbegin() const;

      /*!
       * @return an iterator to after the last column
       */
      iterator end();

      /*!
       * @return a const iterator to after the last column
       */
      const_iterator end() const;

      /*!
       * @return an immutable iterator to after the last column
       */
      const_iterator cend() const;

      /*!
       * gets a column at a position
       * @param  position   the index of the column
       * @return            the column object
       */
      column_type operator[](size_t position) const;

      /*!
       * gets a column by a name
       * @param  name the name of the column
       * @return      the column object
       */
      column_type operator[](const std::string &name) const;

      /*!
       * gets the column name
       * @param  nPosition the index of the column
       * @return           the column name
       */
      std::string column_name(size_t nPosition) const;

      /*!
       * get a column by index
       * @param  nPosition the index of the column
       * @return           the column object
       */
      column_type column(size_t nPosition) const;

      /*!
       * gets a column by name
       * @param  name the name of the column
       * @return      the column object
       */
      column_type column(const std::string &name) const;

      /*!
       * gets the number of columns in the row
       * @return the number of columns
       */
      size_t size() const noexcept;

      /*!
       * tests if this row has no data
       * @return true if the row has no data
       */
      bool empty() const noexcept;

      /*!
       * tests if the implementation is valid
       * @return true if valid
       */
      bool is_valid() const noexcept;

      /*!
       * performs a callback for each column
       * @param funk the callback
       */
      void each(const std::function<void(const db::column &)> &funk) const;

      /*!
       * gets the implementation for this row
       */
      std::shared_ptr<row_impl> impl() const;
    };
}  // namespace coda::db

#endif

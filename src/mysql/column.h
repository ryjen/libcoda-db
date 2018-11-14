/*!
 * @file column.h
 * a mysql specific representation of a column
 */
#ifndef CODA_DB_MYSQL_COLUMN_H
#define CODA_DB_MYSQL_COLUMN_H

#include "../column.h"
#include <mysql/mysql.h>

namespace coda {
  namespace db {
    namespace mysql {
      class binding;

      /*!
       * a mysql specific implementation of a column using fetching
       */
      class column : public column_impl {
        private:
        std::string name_;
        MYSQL_ROW value_;
        std::shared_ptr<MYSQL_RES> res_;
        size_t index_;

        public:
        /*!
         * @param res the result for column meta data
         * @param value the column values for the result
         * @param index the index of the column value
         */
        column(const std::shared_ptr<MYSQL_RES> &res, MYSQL_ROW value,
               size_t index);

        /* non-copyable boilerplate */
        column(const column &other) = delete;
        column(column &&other);
        virtual ~column();
        column &operator=(const column &other) = delete;
        column &operator=(column &&other);

        /* column_impl overrides */
        bool is_valid() const;
        sql_value to_value() const;
        int sql_type() const;
        std::string name() const;
      };

      /*!
       * a mysql specific version of a column using prepared statements
       */
      class stmt_column : public column_impl {
        private:
        std::string name_;
        std::shared_ptr<mysql::binding> value_;
        size_t position_;

        public:
        /*!
         * @param name the name of the column
         * @param bindings the bindings containing the column value
         * @param position the index of the column in the bindings
         */
        stmt_column(const std::string &name,
                    const std::shared_ptr<binding> &bindings, size_t position);

        /* non-copyable boilerplate */
        stmt_column(const stmt_column &other) = delete;
        stmt_column(stmt_column &&other);
        virtual ~stmt_column();
        stmt_column &operator=(const stmt_column &other) = delete;
        stmt_column &operator=(stmt_column &&other);

        /* column_impl overrides */
        bool is_valid() const;
        sql_value to_value() const;
        int sql_type() const;
        std::string name() const;
      };
    } // namespace mysql
  }   // namespace db
} // namespace coda

#endif

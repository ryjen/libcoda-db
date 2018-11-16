/*!
 * @file column.h
 * a mysql specific representation of a column
 */
#ifndef CODA_DB_MYSQL_COLUMN_H
#define CODA_DB_MYSQL_COLUMN_H

#include <mysql/mysql.h>
#include "../column.h"

namespace coda::db::mysql {
      class binding;

      /*!
       * a mysql specific implementation of a column using fetching
       */
      class column : public column_impl {
       private:
        MYSQL_ROW value_;
        std::shared_ptr<MYSQL_RES> res_;
        unsigned int index_;

       public:
        /*!
         * @param res the result for column meta data
         * @param value the column values for the result
         * @param index the index of the column value
         */
        column(const std::shared_ptr<MYSQL_RES> &res, MYSQL_ROW value, unsigned int index);

        /* non-copyable boilerplate */
        column(const column &other) = delete;
        column(column &&other) noexcept;
        ~column() = default;
        column &operator=(const column &other) = delete;
        column &operator=(column &&other) noexcept;

        /* column_impl overrides */
        bool is_valid() const override;
        sql_value to_value() const override;
        int sql_type() const;
        std::string name() const override;
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
        stmt_column(const std::string &name, const std::shared_ptr<binding> &bindings, size_t position);

        /* non-copyable boilerplate */
        stmt_column(const stmt_column &other) = delete;
        stmt_column(stmt_column &&other) noexcept;
        ~stmt_column() = default;
        stmt_column &operator=(const stmt_column &other) = delete;
        stmt_column &operator=(stmt_column &&other) noexcept;

        /* column_impl overrides */
        bool is_valid() const override;
        sql_value to_value() const override;
        int sql_type() const;
        std::string name() const override;
      };
}  // namespace coda::db::mysql

#endif

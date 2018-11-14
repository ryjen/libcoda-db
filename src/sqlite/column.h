/*!
 * @file column.h
 */
#ifndef CODA_DB_SQLITE_COLUMN_H
#define CODA_DB_SQLITE_COLUMN_H

#include "../column.h"
#include <sqlite3.h>

namespace coda {
  namespace db {
    namespace sqlite {
      /*!
       * a sqlite specific implementation of a column
       */
      class column : public column_impl {
        private:
        std::shared_ptr<sqlite3_stmt> stmt_;
        int column_;

        public:
        /*!
         * @param stmt      the statement in use
         * @param column    the column index
         */
        column(const std::shared_ptr<sqlite3_stmt> &stmt, int column);

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
    } // namespace sqlite
  }   // namespace db
} // namespace coda

#endif

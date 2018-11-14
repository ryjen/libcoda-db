/*!
 * @file resultset.h
 */
#ifndef CODA_DB_SQLITE_RESULTSET_H
#define CODA_DB_SQLITE_RESULTSET_H

#include "../resultset.h"
#include <sqlite3.h>
#include <vector>

namespace coda {
  namespace db {
    namespace sqlite {
      class session;

      /*!
       * a sqlite specific implmentation of a result set
       */
      class resultset : public resultset_impl {
        template <typename, typename> friend class resultset_iterator;

        private:
        std::shared_ptr<sqlite3_stmt> stmt_;
        std::shared_ptr<sqlite::session> sess_;
        int status_;

        public:
        /*!
         * @param  db   the database in use
         * @param  stmt the query statement in use
         */
        resultset(const std::shared_ptr<sqlite::session> &sess,
                  const std::shared_ptr<sqlite3_stmt> &stmt);

        /* non-copyable boilerplate */
        resultset(const resultset &other) = delete;
        resultset(resultset &&other);
        virtual ~resultset();
        resultset &operator=(const resultset &other) = delete;
        resultset &operator=(resultset &&other);

        /* resultset_impl overrides */
        bool is_valid() const noexcept;
        row_type current_row();
        /**
         * @throws database_exception if unable to reset statement
         */
        void reset();
        bool next();
      };
    } // namespace sqlite
  }   // namespace db
} // namespace coda

#endif

#ifndef ARG3_DB_SQLITE_RESULTSET_H_
#define ARG3_DB_SQLITE_RESULTSET_H_

#include "resultset.h"

namespace arg3
{
    namespace db
    {

        class sqlite3_db;

        /*!
         * a sqlite specific implmentation of a result set
         */
        class sqlite3_resultset : public resultset_impl
        {
            friend class select_query;
            friend class row;
            friend class sqldb;
            friend class resultset_iterator;
        private:
            sqlite3_stmt *stmt_;

            sqlite3_db *db_;

            int status_;

        public:
            sqlite3_resultset(sqlite3_db *db, sqlite3_stmt *stmt);

            sqlite3_resultset(const sqlite3_resultset &other) = delete;
            sqlite3_resultset(sqlite3_resultset &&other);
            virtual ~sqlite3_resultset();

            sqlite3_resultset &operator=(const sqlite3_resultset &other) = delete;
            sqlite3_resultset &operator=(sqlite3_resultset && other);

            bool is_valid() const;

            row current_row();

            void reset();

            bool next();

            size_t column_count() const;
        };
    }
}

#endif

#ifndef ARG3_DB_SQLITE_ROW_H_
#define ARG3_DB_SQLITE_ROW_H_

#include "row.h"

namespace arg3
{
    namespace db
    {

        class sqlite3_db;

        class sqlite3_row : public row_impl
        {
            friend class sqlite3_resultset;
        private:
            sqlite3_stmt *stmt_;
            sqlite3_db *db_;
            size_t size_;
        public:
            sqlite3_row(sqlite3_db *db, sqlite3_stmt *stmt);
            virtual ~sqlite3_row();
            sqlite3_row(const sqlite3_row &other);
            sqlite3_row(sqlite3_row &&other);
            sqlite3_row &operator=(const sqlite3_row &other);
            sqlite3_row &operator=(sqlite3_row && other);

            string column_name(size_t nPosition) const;
            arg3::db::column column(size_t nPosition) const;
            arg3::db::column column(const string &name) const;
            size_t size() const;
        };
    }
}

#endif

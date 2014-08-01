#ifndef ARG3_DB_SQLITE_ROW_H_
#define ARG3_DB_SQLITE_ROW_H_

#include "row.h"
#include "sqlite3_column.h"
#include <vector>

#define sqlite3_default_row arg3::db::sqlite3_cached_row

namespace arg3
{
    namespace db
    {

        class sqlite3_db;

        /*!
         *  a sqlite specific implementation of a row
         */
        class sqlite3_row : public row_impl
        {
            friend class sqlite3_resultset;
        private:
            shared_ptr<sqlite3_stmt> stmt_;
            sqlite3_db *db_;
            size_t size_;
        public:
            sqlite3_row(sqlite3_db *db, shared_ptr<sqlite3_stmt> stmt);
            virtual ~sqlite3_row();
            sqlite3_row(const sqlite3_row &other) = delete;
            sqlite3_row(sqlite3_row &&other);
            sqlite3_row &operator=(const sqlite3_row &other) = delete;
            sqlite3_row &operator=(sqlite3_row && other);

            string column_name(size_t nPosition) const;
            arg3::db::column column(size_t nPosition) const;
            arg3::db::column column(const string &name) const;
            size_t size() const;
            bool is_valid() const;
        };

        class sqlite3_cached_row : public row_impl
        {
            friend class sqlite3_resultset;
        private:
            vector<shared_ptr<sqlite3_cached_column>> columns_;
        public:
            sqlite3_cached_row(sqlite3_db *db, shared_ptr<sqlite3_stmt> stmt);
            virtual ~sqlite3_cached_row();
            sqlite3_cached_row(const sqlite3_cached_row &other) = delete;
            sqlite3_cached_row(sqlite3_cached_row &&other);
            sqlite3_cached_row &operator=(const sqlite3_cached_row &other) = delete;
            sqlite3_cached_row &operator=(sqlite3_cached_row && other);

            string column_name(size_t nPosition) const;
            arg3::db::column column(size_t nPosition) const;
            arg3::db::column column(const string &name) const;
            size_t size() const;
            bool is_valid() const;
        };
    }
}

#endif

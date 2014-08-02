#ifndef ARG3_DB_MYSQL_ROW_H_
#define ARG3_DB_MYSQL_ROW_H_

#include "config.h"

#ifdef HAVE_LIBMYSQLCLIENT

#include <mysql/mysql.h>
#include "row.h"
#include "mysql_column.h"
#include <vector>

namespace arg3
{
    namespace db
    {

        class mysql_db;
        class mysql_binding;

        /*!
         *  a mysql specific implementation of a row
         */
        class mysql_row : public row_impl
        {
            friend class mysql_resultset;
        private:
            MYSQL_ROW row_;
            shared_ptr<MYSQL_RES> res_;
            mysql_db *db_;
            size_t size_;
        public:
            mysql_row(mysql_db *db, shared_ptr<MYSQL_RES> res, MYSQL_ROW row);
            virtual ~mysql_row();
            mysql_row(const mysql_row &other);
            mysql_row(mysql_row &&other);
            mysql_row &operator=(const mysql_row &other);
            mysql_row &operator=(mysql_row && other);

            string column_name(size_t nPosition) const;
            arg3::db::column column(size_t nPosition) const;
            arg3::db::column column(const string &name) const;
            size_t size() const;
            bool is_valid() const;
        };

        /*!
         *  a mysql specific implementation of a row using prepared statements
         */
        class mysql_stmt_row : public row_impl
        {
            friend class mysql_stmt_resultset;
        private:
            shared_ptr<mysql_binding> fields_;
            shared_ptr<MYSQL_RES> metadata_;
            mysql_db *db_;
            size_t size_;
        public:
            mysql_stmt_row(mysql_db *db, shared_ptr<MYSQL_RES> metadata, shared_ptr<mysql_binding> fields);
            virtual ~mysql_stmt_row();
            mysql_stmt_row(const mysql_stmt_row &other);
            mysql_stmt_row(mysql_stmt_row &&other);
            mysql_stmt_row &operator=(const mysql_stmt_row &other);
            mysql_stmt_row &operator=(mysql_stmt_row && other);

            string column_name(size_t nPosition) const;
            arg3::db::column column(size_t nPosition) const;
            arg3::db::column column(const string &name) const;
            size_t size() const;
            bool is_valid() const;
        };

        class mysql_cached_row : public row_impl
        {
            friend class mysql_stmt_resultset;
        private:
            std::vector<shared_ptr<mysql_cached_column>> columns_;
        public:
            mysql_cached_row(sqldb *db, shared_ptr<MYSQL_RES> metadata, shared_ptr<mysql_binding> fields);
            mysql_cached_row(sqldb *db, shared_ptr<MYSQL_RES> res, MYSQL_ROW row);
            virtual ~mysql_cached_row() = default;
            mysql_cached_row(const mysql_cached_row &other) = default;
            mysql_cached_row(mysql_cached_row &&other) = default;
            mysql_cached_row &operator=(const mysql_cached_row &other) = default;
            mysql_cached_row &operator=(mysql_cached_row && other) = default;

            string column_name(size_t nPosition) const;
            arg3::db::column column(size_t nPosition) const;
            arg3::db::column column(const string &name) const;
            size_t size() const;
            bool is_valid() const;
        };

    }
}

#endif

#endif

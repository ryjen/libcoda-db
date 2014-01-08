#ifndef ARG3_DB_MYSQL_ROW_H_
#define ARG3_DB_MYSQL_ROW_H_

#include "config.h"

#ifdef HAVE_LIBMYSQLCLIENT

#include <mysql/mysql.h>
#include "row.h"

namespace arg3
{
    namespace db
    {

        class mysql_db;

        /*!
         *  a mysql specific implementation of a row
         */
        class mysql_row : public row_impl
        {
            friend class mysql_resultset;
        private:
            MYSQL_ROW row_;
            MYSQL_RES *res_;
            mysql_db *db_;
            size_t size_;
        public:
            mysql_row(mysql_db *db, MYSQL_RES *res, MYSQL_ROW row);
            virtual ~mysql_row();
            mysql_row(const mysql_row &other);
            mysql_row(mysql_row &&other);
            mysql_row &operator=(const mysql_row &other);
            mysql_row &operator=(mysql_row && other);

            string column_name(size_t nPosition) const;
            arg3::db::column column(size_t nPosition) const;
            arg3::db::column column(const string &name) const;
            size_t size() const;
        };

        /*!
         *  a mysql specific implementation of a row using prepared statements
         */
        class mysql_stmt_row : public row_impl
        {
            friend class mysql_stmt_resultset;
        private:
            MYSQL_BIND *fields_;
            MYSQL_RES *metadata_;
            mysql_db *db_;
            size_t size_;
        public:
            mysql_stmt_row(mysql_db *db, MYSQL_RES *metadata, MYSQL_BIND *fields);
            virtual ~mysql_stmt_row();
            mysql_stmt_row(const mysql_stmt_row &other);
            mysql_stmt_row(mysql_stmt_row &&other);
            mysql_stmt_row &operator=(const mysql_stmt_row &other);
            mysql_stmt_row &operator=(mysql_stmt_row && other);

            string column_name(size_t nPosition) const;
            arg3::db::column column(size_t nPosition) const;
            arg3::db::column column(const string &name) const;
            size_t size() const;
        };
    }
}

#endif

#endif

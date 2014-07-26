#ifndef ARG3_DB_MYSQL_RESULTSET_H_
#define ARG3_DB_MYSQL_RESULTSET_H_

#include "config.h"

#ifdef HAVE_LIBMYSQLCLIENT

#include <mysql/mysql.h>
#include "resultset.h"
#include <vector>

namespace arg3
{
    namespace db
    {

        class mysql_db;
        class mysql_binding;

        /*!
         * a mysql specific implementation of a result set
         */
        class mysql_resultset : public resultset_impl
        {
            friend class select_query;
            friend class row;
            friend class sqldb;
            friend class resultset_iterator;
        private:
            MYSQL_RES *res_;
            MYSQL_ROW row_;
            mysql_db *db_;
        public:
            mysql_resultset(mysql_db *db, MYSQL_RES *res);

            mysql_resultset(const mysql_resultset &other) = delete;
            mysql_resultset(mysql_resultset &&other);
            virtual ~mysql_resultset();

            mysql_resultset &operator=(const mysql_resultset &other) = delete;
            mysql_resultset &operator=(mysql_resultset && other);

            bool is_valid() const;
            row current_row();
            void reset();
            bool next();
            size_t size() const;
        };

        /*!
         * a mysql specific implementation of a result set using prepared statements
         */
        class mysql_stmt_resultset : public resultset_impl
        {
            friend class select_query;
            friend class row;
            friend class sqldb;
            friend class resultset_iterator;
        private:
            MYSQL_STMT *stmt_;
            MYSQL_RES *metadata_;
            mysql_db *db_;
            shared_ptr<mysql_binding> bindings_;
            int status_;
            void prepare_results();
        public:
            mysql_stmt_resultset(mysql_db *db, MYSQL_STMT *stmt);

            mysql_stmt_resultset(const mysql_stmt_resultset &other) = delete;
            mysql_stmt_resultset(mysql_stmt_resultset &&other);
            virtual ~mysql_stmt_resultset();

            mysql_stmt_resultset &operator=(const mysql_stmt_resultset &other) = delete;
            mysql_stmt_resultset &operator=(mysql_stmt_resultset && other);

            bool is_valid() const;

            row current_row();

            void reset();

            bool next();

            size_t size() const;
        };

        class mysql_cached_resultset : public resultset_impl
        {
            friend class select_query;
            friend class row;
            friend class sqldb;
            friend class resultset_iterator;
        private:
            vector<shared_ptr<row_impl>> rows_;
            int currentRow_;
        public:
            mysql_cached_resultset(MYSQL_STMT *stmt);
            mysql_cached_resultset(mysql_db *db, MYSQL_RES *res);

            mysql_cached_resultset(const mysql_cached_resultset &other) = delete;
            mysql_cached_resultset(mysql_cached_resultset &&other);
            virtual ~mysql_cached_resultset();

            mysql_cached_resultset &operator=(const mysql_cached_resultset &other) = delete;
            mysql_cached_resultset &operator=(mysql_cached_resultset && other);

            bool is_valid() const;

            row current_row();

            void reset();

            bool next();

            size_t size() const;
        };
    }
}

#endif

#endif

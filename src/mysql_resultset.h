/*!
 * @file mysql_resultset.h
 * representation of results from a mysql query
 */
#ifndef ARG3_DB_MYSQL_RESULTSET_H
#define ARG3_DB_MYSQL_RESULTSET_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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
            template <typename, typename>
            friend class resultset_iterator;

           private:
            std::shared_ptr<MYSQL_RES> res_;
            MYSQL_ROW row_;
            mysql_db *db_;

           public:
            /*!
             * @param db the database in use
             * @param res the query results
             */
            mysql_resultset(mysql_db *db, const std::shared_ptr<MYSQL_RES> &res);

            /* non-copyable boilerplate */
            mysql_resultset(const mysql_resultset &other) = delete;
            mysql_resultset(mysql_resultset &&other);
            mysql_resultset &operator=(const mysql_resultset &other) = delete;
            mysql_resultset &operator=(mysql_resultset &&other);
            virtual ~mysql_resultset();

            /* resultset_impl overrides */
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
            template <typename, typename>
            friend class resultset_iterator;

           private:
            std::shared_ptr<MYSQL_STMT> stmt_;
            std::shared_ptr<MYSQL_RES> metadata_;
            mysql_db *db_;
            std::shared_ptr<mysql_binding> bindings_;
            int status_;
            void prepare_results();

           public:
            /*!
             * @param db the database in use
             * @param stmt the statement being executed
             */
            mysql_stmt_resultset(mysql_db *db, const std::shared_ptr<MYSQL_STMT> &stmt);

            /* non-copyable boilerplate */
            mysql_stmt_resultset(const mysql_stmt_resultset &other) = delete;
            mysql_stmt_resultset &operator=(const mysql_stmt_resultset &other) = delete;
            mysql_stmt_resultset(mysql_stmt_resultset &&other);
            mysql_stmt_resultset &operator=(mysql_stmt_resultset &&other);

            /* boilerplate */
            virtual ~mysql_stmt_resultset();

            /* resultset_impl overrides */
            bool is_valid() const;
            row current_row();
            void reset();
            bool next();
            size_t size() const;
        };

        namespace helper
        {
            /*! helper to clean up after a query result */
            struct mysql_res_delete {
                void operator()(MYSQL_RES *p) const;
            };
        }
    }
}

#endif

#endif

/*!
 * @file db.h
 * a mysql specific database
 */
#ifndef ARG3_DB_MYSQL_SQLDB_H
#define ARG3_DB_MYSQL_SQLDB_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBMYSQLCLIENT

#include <mysql/mysql.h>
#include "../sqldb.h"

namespace arg3
{
    namespace db
    {
        namespace mysql
        {
            /*!
             * a mysql specific implementation of a database
             */
            class db : public sqldb
            {
                friend statement;
                friend resultset;

               protected:
                std::shared_ptr<MYSQL> db_;
                int flags_;

               public:
                constexpr static int CACHE_RESULTS = (1 << 0);
                constexpr static int CACHE_STATEMENTS = (1 << 1);
                constexpr static int CACHE = CACHE_RESULTS | CACHE_STATEMENTS;

                /*!
                 * default constructor takes a uri to connect to
                 * @param connInfo the uri connection info
                 */
                db(const uri &connInfo);

                /* boilerplate */
                db(const db &other);
                db(db &&other);
                db &operator=(const db &other);
                db &operator=(db &&other);
                virtual ~db();

                db &flags(int value);

                int flags() const;

                /* sqldb overrides */
                bool is_open() const;
                void open();
                void close();
                long long last_insert_id() const;
                int last_number_of_changes() const;
                std::string last_error() const;
                resultset_type execute(const std::string &sql);
                std::shared_ptr<statement_type> create_statement();
            };
        }
    }
}

#endif

#endif

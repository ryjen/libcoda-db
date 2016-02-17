/*!
 * @file db.h
 * a postgres database
 */
#ifndef POSTGRES_DB_H
#define POSTGRES_DB_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include <libpq-fe.h>
#include "../sqldb.h"

namespace arg3
{
    namespace db
    {
        namespace postgres
        {
            /*!
             * a mysql specific implementation of a database
             */
            class db : public sqldb
            {
                friend class statement;
                friend class resultset;

               protected:
                std::shared_ptr<PGconn> db_;

               public:
                /*!
                 * @param info the connection uri
                 */
                db(const uri &info);

                /* boilerplate */
                db(const db &other);
                db(db &&other);
                db &operator=(const db &other);
                db &operator=(db &&other);
                virtual ~db();

                /* sqldb overrides */
                bool is_open() const;
                void open();
                void close();
                long long last_insert_id() const;
                int last_number_of_changes() const;
                std::string last_error() const;
                resultset_type execute(const std::string &sql);
                std::shared_ptr<statement_type> create_statement();
                std::string insert_sql(const std::shared_ptr<schema> &schema, const std::vector<std::string> &columns) const;

               private:
                long long lastId_;
                int lastNumChanges_;
                void set_last_insert_id(long long value);
                void set_last_number_of_changes(int value);
            };

            /*!
             * utility to cleanup a postgres result
             */
            namespace helper
            {
                struct res_delete {
                    void operator()(PGresult *p) const;
                };
            }
        }
    }
}


#endif
#endif

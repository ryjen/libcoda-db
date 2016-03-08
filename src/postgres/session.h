/*!
 * @file db.h
 * a postgres database
 */
#ifndef ARG3_DB_POSTGRES_SESSION_H
#define ARG3_DB_POSTGRES_SESSION_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include <libpq-fe.h>
#include "../session.h"
#include "../session_factory.h"

namespace arg3
{
    namespace db
    {
        namespace postgres
        {
            class factory : public session_factory
            {
               public:
                arg3::db::session *create(const uri &uri);
            };

            /*!
             * a mysql specific implementation of a database
             */
            class session : public arg3::db::session
            {
                friend class statement;
                friend class factory;

               protected:
                std::shared_ptr<PGconn> db_;

                /*!
                 * @param info the connection uri
                 */
                session(const uri &info);

               public:
                /* boilerplate */
                session(const session &other) = delete;
                session(session &&other);
                session &operator=(const session &other) = delete;
                session &operator=(session &&other);
                virtual ~session();

                /* sqldb overrides */
                bool is_open() const;
                void open();
                void close();
                long long last_insert_id() const;
                int last_number_of_changes() const;
                std::string last_error() const;
                arg3::db::session::resultset_type query(const std::string &sql);
                bool execute(const std::string &sql);
                std::shared_ptr<arg3::db::session::statement_type> create_statement();
                arg3::db::session::transaction_type create_transaction();
                void query_schema(const std::string &tablename, std::vector<column_definition> &columns);
                std::string insert_sql(const std::shared_ptr<schema> &schema, const std::vector<std::string> &columns) const;

               private:
                long long lastId_;
                int lastNumChanges_;
                void set_last_insert_id(long long value);
                void set_last_number_of_changes(int value);
            };
        }
    }
}

#endif

#endif

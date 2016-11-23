/*!
 * @file db.h
 * a postgres database
 */
#ifndef RJ_DB_POSTGRES_SESSION_H
#define RJ_DB_POSTGRES_SESSION_H

#include <libpq-fe.h>
#include "../session.h"
#include "../session_factory.h"
#include "transaction.h"

namespace rj
{
    namespace db
    {
        namespace postgres
        {
            __attribute__((constructor)) void initialize(void);

            class factory : public session_factory
            {
               public:
                std::shared_ptr<rj::db::session_impl> create(const uri &uri);
            };

            /*!
             * a mysql specific implementation of a database
             */
            class session : public rj::db::session_impl, public std::enable_shared_from_this<session>
            {
                friend class statement;
                friend class factory;

               protected:
                std::shared_ptr<PGconn> db_;

               public:
                /*!
                 * @param info the connection uri
                 */
                session(const uri &info);

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
                std::shared_ptr<resultset_impl> query(const std::string &sql);
                bool execute(const std::string &sql);
                std::shared_ptr<rj::db::session::statement_type> create_statement();
                std::shared_ptr<transaction_impl> create_transaction() const;
                std::shared_ptr<transaction_impl> create_transaction(const transaction::mode &mode) const;
                std::vector<column_definition> get_columns_for_schema(const std::string &dbName, const std::string &tablename);
                std::string get_insert_sql(const std::shared_ptr<schema> &schema, const std::vector<std::string> &columns) const;
                std::string bind_param(size_t index) const;
                int features() const;

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

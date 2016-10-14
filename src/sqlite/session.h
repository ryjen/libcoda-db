/*!
 * @file session.h
 */
#ifndef RJ_DB_SQLITE_SESSION_H
#define RJ_DB_SQLITE_SESSION_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBSQLITE3

#include <sqlite3.h>
#include "../session.h"
#include "../session_factory.h"
#include "transaction.h"

namespace rj
{
    namespace db
    {
        namespace sqlite
        {
            class factory : public session_factory
            {
               public:
                std::shared_ptr<rj::db::session_impl> create(const uri &uri);
            };

            /*!
             * a sqlite specific implementation of a database
             */
            class session : public rj::db::session_impl, public std::enable_shared_from_this<session>
            {
                friend class factory;
                friend class statement;

               protected:
                std::shared_ptr<sqlite3> db_;

               public:
                /*!
                 * @param info   the connection info
                 */
                session(const uri &info);

                /* boilerplate */
                session(const session &other) = delete;
                session(session &&other);
                session &operator=(const session &other) = delete;
                session &operator=(session &&other);
                virtual ~session();

                /* sqlsession overrides */
                bool is_open() const;
                void open(int flags);
                void open();
                void close();
                long long last_insert_id() const;
                int last_number_of_changes() const;
                std::shared_ptr<resultset_impl> query(const std::string &sql);
                bool execute(const std::string &sql);
                std::string last_error() const;
                std::shared_ptr<statement_type> create_statement();
                std::shared_ptr<transaction_impl> create_transaction() const;
                std::shared_ptr<transaction_impl> create_transaction(transaction::type type) const;

                /*! @copydoc
                 *  overriden for sqlite3 specific pragma parsing
                 */
                std::vector<column_definition> get_columns_for_schema(const std::string &dbName, const std::string &tableName);
                std::string bind_param(size_t index) const;

                bool supports_named_parameters() const;
            };
        }
    }
}

#endif

#endif

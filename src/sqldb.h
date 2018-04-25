#ifndef CODA_DB_SQLDB_H
#define CODA_DB_SQLDB_H

#include "uri.h"
#include <memory>

namespace coda {
    namespace db {
        class session;

        class session_factory;

            /*!
             * parses a uri and creates a session
             * @param value   the uri string to parse
             * @return the database session based on the uri
             */
            std::shared_ptr<coda::db::session> create_session(const std::string &value);

            /*!
             * creates a database session
             * @param value the uri for the session
             * @return the created database session
             */
            std::shared_ptr<coda::db::session> create_session(const uri &value);

            /*!
             * parses a uri, creates a session, and opens it
             * @param value the uri for the session
             * @return the created and opened session
             */
            std::shared_ptr<coda::db::session> open_session(const std::string &value);

            /*!
             * creates a session and opens it
             * @param value the uri for the session
             * @return the created and opened session
             */
            std::shared_ptr<coda::db::session> open_session(const uri &value);

            /*!
             * registers a handler for a session protocol
             * ex. "sqlite", my_sqlite_factory
             * @param protocol the protocol, or the 'scheme' of a uri
             * @param factory  a factory instance
             */
            void register_session(const std::string &protocol, const std::shared_ptr<session_factory> &factory);
    }
}

#endif

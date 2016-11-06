#ifndef RJ_DB_SQLDB_H
#define RJ_DB_SQLDB_H

#include <memory>
#include <unordered_map>
#include "session.h"
#include "session_factory.h"
#include "uri.h"

namespace rj
{
    namespace db
    {
        class session;

        namespace log
        {
            /*! levels of logging */
            typedef enum {
                /*! logging is disabled */
                None = 0,
                /*! only error messages will be logged */
                Error = 1,
                /*! warnings and errors will be logged */
                Warn = 2,
                /*! info, warning, and error messages will be logged */
                Info = 3,
                /*! debug, info, warning and error messages will be logged */
                Debug = 4,
                /*! trace, debug, info, warning and error messages will be logged */
                Trace = 5
            } level;

            /*!
             * sets the logging level
             * @param level the level to set
             */
            void set_level(level level);

            /*!
             * sets the log level from a name
             * @param arg a string representation of one of the log levels
             */
            void set_level(const char *arg);
        }

        class sqldb
        {
           public:
            /*!
             * parses a uri and creates a session
             * @param value   the uri string to parse
             * @return the database session based on the uri
             */
            static std::shared_ptr<rj::db::session> create_session(const std::string &value);

            /*!
             * creates a database session
             * @param value the uri for the session
             * @return the created database session
             */
            static std::shared_ptr<rj::db::session> create_session(const uri &value);

            /*!
             * parses a uri, creates a session, and opens it
             * @param value the uri for the session
             * @return the created and opened session
             */
            static std::shared_ptr<rj::db::session> open_session(const std::string &value);

            /*!
             * creates a session and opens it
             * @param value the uri for the session
             * @return the created and opened session
             */
            static std::shared_ptr<rj::db::session> open_session(const uri &value);

            /*!
             * registers a handler for a session protocol
             * ex. "sqlite", my_sqlite_factory
             * @param protocol the protocol, or the 'scheme' of a uri
             * @param factory  a factory instance
             */
            static void register_session(const std::string &protocol, const std::shared_ptr<session_factory> &factory);

           private:
            static sqldb *instance();
            sqldb();
            sqldb(sqldb &&move) = delete;
            sqldb(const sqldb &other) = delete;
            sqldb &operator=(const sqldb &other) = delete;
            sqldb &operator=(sqldb &&other) = delete;
            virtual ~sqldb();
            std::unordered_map<std::string, std::shared_ptr<session_factory>> factories_;
        };
    }
}

#endif

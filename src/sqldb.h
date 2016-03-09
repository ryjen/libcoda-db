#ifndef ARG3_DB_SQLDB_H
#define ARG3_DB_SQLDB_H

#include <unordered_map>
#include <memory>
#include "uri.h"
#include "session_factory.h"
#include "session.h"

namespace arg3
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

        struct sqldb {
           public:
            /*!
             * parses a uri and returns a database object
             * @param value   the uri string to parse
             */
            static arg3::db::session create_session(const std::string &value);

            static arg3::db::session create_session(const uri &value);

            static void register_session(const std::string &protocol, const std::shared_ptr<session_factory> &factory);

            template <typename T>
            static T create_session(const uri &value)
            {
                return dynamic_cast<T>(create_session(value));
            }

            template <typename T>
            static T create_session(const std::string &value)
            {
                return dynamic_cast<T>(create_session(value));
            }

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

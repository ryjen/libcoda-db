/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef ARG3_DB_SQLDB_H
#define ARG3_DB_SQLDB_H

#include <string>
#include <sstream>
#include <type_traits>
#include "sql_value.h"
#include "schema_factory.h"
#include "statement.h"

namespace arg3
{
    namespace db
    {
        class resultset;

        struct uri {
           public:
            uri(const std::string &url)
            {
                parse(url);
            }
            void parse(const std::string &url);
            std::string protocol, user, password, host, port, path, query, value;
            operator std::string() const
            {
                return value;
            }
        };

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
            } Level;

            void set_level(Level level);
        }

        /*!
         *  a base class for a specific implementation of a database
         */
        class sqldb
        {
           public:
            typedef enum { CACHE_NONE, CACHE_RESULTSET, CACHE_ROW, CACHE_COLUMN } CacheLevel;

            static std::shared_ptr<sqldb> from_uri(const std::string &value);

            sqldb(const uri &connectionInfo);
            sqldb(const sqldb &other) = default;
            sqldb(sqldb &&other) = default;
            sqldb &operator=(const sqldb &other) = default;
            sqldb &operator=(sqldb &&other) = default;
            virtual ~sqldb() = default;

            virtual bool is_open() const = 0;

            virtual void open() = 0;
            virtual void close() = 0;

            virtual void query_schema(const std::string &tablename, std::vector<column_definition> &columns) = 0;

            virtual long long last_insert_id() const = 0;

            virtual int last_number_of_changes() const = 0;

            uri connection_info() const;

            virtual resultset execute(const std::string &sql, bool cache) = 0;

            resultset execute(const std::string &sql);

            virtual std::string last_error() const = 0;

            schema_factory *schemas();

            virtual shared_ptr<statement> create_statement() = 0;

            void set_cache_level(CacheLevel level);

            CacheLevel cache_level() const;

           private:
            uri connectionInfo_;
            CacheLevel cacheLevel_;

           protected:
            schema_factory schema_factory_;
        };
    }
}

#endif

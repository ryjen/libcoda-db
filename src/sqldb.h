/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_SQLDB_H_
#define _ARG3_DB_SQLDB_H_

#include <string>
#include <sstream>
#include <sqlite3.h>
#include <type_traits>
#include "sql_value.h"
#include "schema_factory.h"
#include "statement.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        class resultset;

        shared_ptr<sqldb> get_db_from_uri(const string &uri);

        /*!
         *  a base class for a specific implementation of a database
         */
        class sqldb
        {
        public:

            typedef enum
            {
                NONE, INFO, DEBUG, VERBOSE
            } LogLevel;

            sqldb();
            sqldb(const sqldb &other) = default;
            sqldb(sqldb &&other) = default;
            sqldb &operator=(const sqldb &other) = default;
            sqldb &operator=(sqldb && other) = default;
            virtual ~sqldb() = default;

            virtual bool is_open() const = 0;

            virtual void open() = 0;
            virtual void close() = 0;

            virtual void query_schema(const string &tablename, std::vector<column_definition> &columns) = 0;

            virtual long long last_insert_id() const = 0;

            virtual int last_number_of_changes() const = 0;

            virtual string connection_string() const = 0;
            virtual void set_connection_string(const string &value) = 0;

            virtual resultset execute(const string &sql) = 0;

            virtual string last_error() const = 0;

            virtual schema_factory *schemas() = 0;

            virtual shared_ptr<statement> create_statement() = 0;

            void set_log_level(LogLevel level);

            void log(LogLevel level, const string &message);
        private:
            LogLevel logLevel_;
        };

    }
}

#endif

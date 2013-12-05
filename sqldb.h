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

        class sqldb
        {
        public:
            sqldb() = default;
            sqldb(const sqldb &other) = default;
            sqldb(sqldb &&other) = default;
            sqldb &operator=(const sqldb &other) = default;
            sqldb &operator=(sqldb && other) = default;
            virtual ~sqldb() = default;

            virtual bool is_open() const = 0;

            virtual void open() = 0;
            virtual void close() = 0;

            virtual long long last_insert_id() const = 0;

            virtual int last_number_of_changes() const = 0;

            virtual string connection_string() const = 0;
            virtual void set_connection_string(const string &value) = 0;

            virtual resultset execute(const string &sql) = 0;

            virtual string last_error() const = 0;

            virtual schema_factory *schemas() = 0;

            virtual shared_ptr<statement> create_statement() = 0;
        };

        // an instanceof a database
        class sqlite3_db : public sqldb
        {
            friend class base_query;
            friend class sqlite3_statement;
        protected:
            sqlite3 *db_;
        private:
            string filename_;
            schema_factory schema_factory_;
        public:
            sqlite3_db(const string &name = "arg3.db");
            sqlite3_db(const sqlite3_db &other);
            sqlite3_db(sqlite3_db &&other);
            sqlite3_db &operator=(const sqlite3_db &other);
            sqlite3_db &operator=(sqlite3_db && other);
            virtual ~sqlite3_db();

            bool is_open() const;

            void open();
            void close();

            long long last_insert_id() const;

            int last_number_of_changes() const;

            string connection_string() const;
            void set_connection_string(const string &value);

            resultset execute(const string &sql);

            string last_error() const;

            schema_factory *schemas();

            shared_ptr<statement> create_statement();
        };
    }
}

#endif

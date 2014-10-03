#ifndef ARG3_DB_SQLITE_SQLDB_H_
#define ARG3_DB_SQLITE_SQLDB_H_

#include "config.h"

#ifdef HAVE_LIBSQLITE3

#include <sqlite3.h>

#include "sqldb.h"

namespace arg3
{
    namespace db
    {
        /*!
         * a sqlite specific implementation of a database
         */
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

            void open(int flags);

            void open();

            void close();

            long long last_insert_id() const;

            int last_number_of_changes() const;

            void query_schema(const string &tableName, std::vector<column_definition> &columns);

            string connection_string() const;
            void set_connection_string(const string &value);

            resultset execute(const string &sql, bool cache = false);

            string last_error() const;

            schema_factory *schemas();

            shared_ptr<statement> create_statement();
        };
    }
}

#endif

#endif

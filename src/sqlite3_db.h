#ifndef ARG3_DB_SQLITE_SQLDB_H
#define ARG3_DB_SQLITE_SQLDB_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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

           public:
            sqlite3_db(const uri &info);
            sqlite3_db(const sqlite3_db &other);
            sqlite3_db(sqlite3_db &&other);
            sqlite3_db &operator=(const sqlite3_db &other);
            sqlite3_db &operator=(sqlite3_db &&other);
            virtual ~sqlite3_db();

            bool is_open() const;

            void open(int flags);

            void open();

            void close();

            long long last_insert_id() const;

            int last_number_of_changes() const;

            void query_schema(const std::string &tableName, std::vector<column_definition> &columns);

            resultset execute(const std::string &sql, bool cache = false);

            std::string last_error() const;

            std::shared_ptr<statement> create_statement();
        };
    }
}

#endif

#endif

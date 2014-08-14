#ifndef ARG3_DB_MYSQL_SQLDB_H_
#define ARG3_DB_MYSQL_SQLDB_H_

#include "config.h"

#ifdef HAVE_LIBMYSQLCLIENT

#include <mysql/mysql.h>
#include "sqldb.h"

namespace arg3
{
    namespace db
    {
        /*!
         * a mysql specific implementation of a database
         */
        class mysql_db : public sqldb
        {
            friend class base_query;
            friend class mysql_statement;
            friend class mysql_resultset;
            friend class mysql_cached_resultset;
        protected:
            MYSQL *db_;
        private:
            string dbName_;
            string user_;
            string password_;
            string host_;
            int port_;
            schema_factory schema_factory_;
        public:
            mysql_db(const string &dbName, const string &user = "root", const string &password = "", const string &host = "localhost", int port = 3306);
            mysql_db(const mysql_db &other);
            mysql_db(mysql_db &&other);
            mysql_db &operator=(const mysql_db &other);
            mysql_db &operator=(mysql_db && other);
            virtual ~mysql_db();

            bool is_open() const;
            void open();
            void close();
            void query_schema(const string &tablename, std::vector<column_definition> &columns);

            long long last_insert_id() const;
            int last_number_of_changes() const;
            string last_error() const;

            string connection_string() const;
            void set_connection_string(const string &value);

            resultset execute(const string &sql, bool cache = false);

            schema_factory *schemas();

            shared_ptr<statement> create_statement();
        };

        extern string last_stmt_error(MYSQL_STMT *stmt);
    }
}

#endif

#endif

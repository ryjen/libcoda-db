#ifndef _ARG3_DB_SQLDB_H_
#define _ARG3_DB_SQLDB_H_

#include <string>
#include <sstream>
#include <exception>
#include <sqlite3.h>

#include "defines.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        class database_exception : public exception
        {

        };

        class select_query;

        class sqldb
        {
        friend class base_query;
        private:
            sqlite3 *mDb;
            string mFileName;
        public:
            sqldb(const string &name = "arg3.db");
            sqldb(const sqldb &other);
            sqldb &operator=(const sqldb &other);
            ~sqldb();

            void open();
            void close();

            select_query select(const columnset &columns, const string &tablename,
            	const string &where = "", const string &orderBy = "", const string &limit = "", const string &groupBy = "") const;
            
            select_query select(const string &tablename,
            	const string &where = "", const string &orderBy = "", const string &limit = "", const string &groupBy = "") const;

            void execute(const string &sql, sql_callback = NULL);

            string last_error() const;
        };
    }
}

#endif

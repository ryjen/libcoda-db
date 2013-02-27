/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_SQLDB_H_
#define _ARG3_DB_SQLDB_H_

#include <string>
#include <sstream>
#include <sqlite3.h>
#include <type_traits>
//#include "select_query.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        class resultset;
        
        class sqldb
        {
            friend class base_query;
        private:
            sqlite3 *m_db;
            string m_fileName;
        public:
            sqldb(const string &name = "arg3.db");
            sqldb(const sqldb &other);
            sqldb &operator=(const sqldb &other);
            ~sqldb();

            bool is_open() const;

            void open();
            void close();

            resultset execute(const string &sql);

            string last_error() const;
        };
    }
}

#endif

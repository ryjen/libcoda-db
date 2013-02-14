/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_SQLDB_H_
#define _ARG3_DB_SQLDB_H_

#include <string>
#include <sstream>
#include <sqlite3.h>
#include <type_traits>
#include "defines.h"
#include "select_query.h"

using namespace std;

namespace arg3
{
    namespace db
    {

        class select_query;
        class base_record;

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

            void open();
            void close();

            select_query select(const column_definition &columns, const string &tablename,
                                const string &where = "", const string &orderBy = "", const string &limit = "", const string &groupBy = "") const;

            select_query select(const string &tablename,
                                const string &where = "", const string &orderBy = "", const string &limit = "", const string &groupBy = "") const;

            void execute(const string &sql, sql_callback = NULL);

            string last_error() const;


            template<typename T>
            static vector<T> findAll() {
                static_assert(is_base_of<base_record, T>::value, "template argument is not of type base_record");

                return T().findAll();
            }
        };
    }
}

#endif

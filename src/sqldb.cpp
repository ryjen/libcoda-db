/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "sqldb.h"
#include "base_query.h"
#include "exception.h"
#include "resultset.h"
#include "sqlite3_db.h"
#include "mysql_db.h"

namespace arg3
{
    namespace db
    {
        ARG3_IMPLEMENT_EXCEPTION(database_exception, std::exception);

        ARG3_IMPLEMENT_EXCEPTION(no_such_column_exception, database_exception);

        ARG3_IMPLEMENT_EXCEPTION(record_not_found_exception, database_exception);

        ARG3_IMPLEMENT_EXCEPTION(binding_error, database_exception);

        bool prefix(const string &astr, const string &bstr)
        {
            if (astr.length() == 0)
            {
                return false;
            }

            for ( auto a = astr.cbegin(), b = bstr.cbegin();
                    a != astr.cend(); a++, b++ )
            {
                if ( tolower(*a) != tolower(*b) )
                    return false;
            }

            return true;
        }

        shared_ptr<sqldb> get_db_from_uri(const string &uri)
        {
            if (prefix(uri, "file:"))
                return make_shared<sqlite3_db>(uri);
            else if (prefix(uri, "mysql:"))
                return make_shared<mysql_db>(uri);
            else
                throw database_exception("unknown database");
        }

        sqldb::sqldb() : logLevel_(NONE)
        {}

        void sqldb::set_log_level(LogLevel level)
        {
            logLevel_ = level;
        }

        void sqldb::log(LogLevel level, const string &message)
        {
            if (logLevel_ >= level)
                cout << "sqldb: " << message << endl;
        }
    }
}

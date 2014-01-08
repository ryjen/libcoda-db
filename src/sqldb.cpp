/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "sqldb.h"
#include "base_query.h"
#include "exception.h"
#include "resultset.h"

namespace arg3
{
    namespace db
    {
        ARG3_IMPLEMENT_EXCEPTION(database_exception, std::exception);

        ARG3_IMPLEMENT_EXCEPTION(no_such_column_exception, database_exception);

        ARG3_IMPLEMENT_EXCEPTION(record_not_found_exception, database_exception);

        ARG3_IMPLEMENT_EXCEPTION(binding_error, database_exception);

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

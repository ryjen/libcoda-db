/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_EXCEPTION_H_
#define _ARG3_DB_EXCEPTION_H_

#include "../exception/exception.h"
#include <string>

using namespace std;

namespace arg3
{
    namespace db
    {
        ARG3_DECLARE_EXCEPTION(database_exception, base_exception);

        ARG3_DECLARE_EXCEPTION(no_such_column_exception, database_exception);

        ARG3_DECLARE_EXCEPTION(record_not_found_exception, database_exception);

        ARG3_DECLARE_EXCEPTION(binding_error, database_exception);
    }
}

#endif

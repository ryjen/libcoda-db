/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_DEFINES_H_
#define _ARG3_DB_DEFINES_H_

#include <map>
#include <vector>
#include <string>

using namespace std;

namespace arg3
{
    namespace db
    {
        typedef map<string,int> column_definition;

        typedef int (*sql_callback)(void *, int, char **, char **);
    }
}

#endif

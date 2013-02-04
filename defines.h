#ifndef _ARG3_DB_DEFINES_H_
#define _ARG3_DB_DEFINES_H_

#include <map>
#include <vector>
#include <string>

#include "../variant/variant.h"

namespace arg3
{
    namespace db
    {

        typedef map<string, variant> row;

        typedef vector<row> rowset;

        typedef vector<string> columnset;

        typedef int (*sql_callback)(void *, int, char **, char **);
    }
}

#endif

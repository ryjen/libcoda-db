#include <igloo/igloo.h>
#include "sql_value.h"

using namespace igloo;

using namespace std;

using namespace arg3::db;

Context(sql_value_test)
{

    Spec(can_convert)
    {
        sql_value v = "1234";

        Assert::That(v, Equals(1234));
    }

};

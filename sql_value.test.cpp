#include <igloo/igloo.h>
#include "sql_value.h"

using namespace igloo;

using namespace std;

using namespace arg3::db;

Context(sql_value_test)
{

    Spec(equality)
    {
        sql_value v = "1234";

        Assert::That(v, Equals(1234));

        Assert::That(v, Equals("1234"));

        sql_value a = "1234";

        sql_value b = 1234;

        Assert::That(a, Equals(b));
    }

};

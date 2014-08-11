#include <bandit/bandit.h>
#include "sql_value.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

go_bandit([]()
{
    describe("sql_value", []()
    {
        it("has equality", []()
        {
            sql_value v = "1234";

            Assert::That(v, Equals(1234));

            Assert::That(v, Equals("1234"));

            sql_value a = "1234";

            sql_value b = 1234;

            Assert::That(a, Equals(b));
        });

    });

});


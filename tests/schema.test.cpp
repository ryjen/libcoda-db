#include <igloo/igloo.h>
#include "schema.h"
#include "db.test.h"

using namespace igloo;

using namespace std;

using namespace arg3::db;

Context(schema_test)
{
    static void SetUpContext()
    {
        setup_testdb();

    }

    static void TearDownContext()
    {
        teardown_testdb();
    }


    Spec(test_primary_keys)
    {
        user u;

        auto keys = u.schema()->primary_keys();

        Assert::That(keys.size(), Equals(1));

        Assert::That(keys[0], Equals("id"));
    }

};

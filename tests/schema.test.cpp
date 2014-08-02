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

    Spec(operators)
    {
        schema s(testdb, "users");

        s.init();

        schema other(std::move(s));

        Assert::That(s.is_valid(), Equals(false));

        Assert::That(other.is_valid(), Equals(true));

        schema copy(testdb, "other_users");

        copy = other;

        Assert::That(copy.table_name(), Equals(other.table_name()));

        schema moved(testdb, "moved_users");

        moved = std::move(other);

        Assert::That(moved.is_valid(), Equals(true));

        Assert::That(other.is_valid(), Equals(false));
    }

    Spec(columns)
    {
        schema s(testdb, "users");

        s.init();

        auto cd = s[0];

        ostringstream os;

        os << cd;

        Assert::That(os.str(), Equals("id"));
    }
};

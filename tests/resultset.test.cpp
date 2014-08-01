#include <igloo/igloo.h>
#include "resultset.h"
#include "db.test.h"

using namespace igloo;

using namespace std;

using namespace arg3::db;

Context(resultset_test)
{
    user user1;
    user user2;

    static void SetUpContext()
    {
        setup_testdb();

    }

    static void TearDownContext()
    {
        teardown_testdb();
    }

    void SetUp()
    {
        user1.set("first_name", "Bryan");
        user1.set("last_name", "Jenkins");

        user1.save();

        user2.set("first_name", "Mark");
        user2.set("last_name", "Smith");

        user2.save();
    }

    void TearDown()
    {
        user1.de1ete();
        user2.de1ete();
    }

    Spec(movable)
    {
        auto rs = testdb->execute("select * from users");

        Assert::That(rs.is_valid(), Equals(true));

        resultset rs2(std::move(rs));

        Assert::That(rs2.is_valid(), Equals(true));

        Assert::That(rs.is_valid(), Equals(false));
    }
};


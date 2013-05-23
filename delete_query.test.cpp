#include <igloo/igloo.h>
#include "base_record.h"
#include "delete_query.h"
#include "db.test.h"

using namespace igloo;

using namespace std;

using namespace arg3::db;

Context(delete_query_test)
{
    static void SetUpContext()
    {
        testdb.setup();

        user user1;

        user1.set("id", 1);
        user1.set("first_name", "Bryan");
        user1.set("last_name", "Jenkins");

        user1.save();


        user user2;

        user2.set("id", 3);

        user2.set("first_name", "Bob");
        user2.set("last_name", "Smith");

        user2.save();
    }

    static void TearDownContext()
    {
        testdb.teardown();
    }

    Spec(delete_test)
    {
        delete_query query(&testdb, "users");

        query.where("first_name=?");

        query.bind(1, "Bob");

        Assert::That(query.execute(), Equals(true));

        Assert::That(testdb.lastNumberOfChanges(), Equals(1));
    }
};

#include <igloo/igloo.h>
#include "base_record.h"
#include "delete_query.h"
#include "db.test.h"

using namespace igloo;

using namespace std;

using namespace arg3::db;

Context(modify_query_test)
{
    static void SetUpContext()
    {
        testdb.setup();

        user user1;

        user1.set_id(1);
        user1.set("first_name", "Bryan");
        user1.set("last_name", "Jenkins");

        user1.save();


        user user2;

        user2.set_id(3);

        user2.set("first_name", "Bob");
        user2.set("last_name", "Smith");

        user2.save();
    }

    static void TearDownContext()
    {
        testdb.teardown();
    }

    Spec(modify_test)
    {
        modify_query query(&testdb, "users", { "id", "first_name", "last_name" });

        query.bind(1, 1);
        query.bind(2, "blah");
        query.bind(3, "bleh");

        Assert::That(query.execute(), Equals(true));

        user u1(1);

        Assert::That(u1.refresh(), Equals(true));
    };
};
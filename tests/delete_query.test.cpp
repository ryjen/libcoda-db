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
        setup_testdb();

        user user1;
        user1.set("first_name", "Bryan");
        user1.set("last_name", "Jenkins");

        user1.save();

        user user2;

        user2.set("first_name", "Mark");
        user2.set("last_name", "Smith");

        user2.save();
    }

    static void TearDownContext()
    {
        teardown_testdb();
    }

    Spec(delete_test)
    {
        delete_query query(testdb, "users");

        query.where("first_name = ?");

        query.bind(1, "Mark");

        Assert::That(query.execute(), Equals(1));

        //Assert::That(query.last_number_of_changes(), Equals(1));
    }
};

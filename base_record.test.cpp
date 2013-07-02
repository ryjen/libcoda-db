#include <igloo/igloo.h>
#include "base_record.h"
#include "db.test.h"

using namespace igloo;

using namespace std;

using namespace arg3::db;

Context(base_record_test)
{
    static void SetUpContext()
    {
        testdb.setup();

    }

    static void TearDownContext()
    {
        testdb.teardown();
    }

    Spec(save_test)
    {
        try
        {
            user user1;

            user1.set("id", 1);
            user1.set("first_name", "Ryan");
            user1.set("last_name", "Jennings");

            Assert::That(user1.save(), Equals(true));

            user1.loadBy("id", 1); // load values back up from db

            Assert::That(user1.get("first_name"), Equals("Ryan"));

            user1.set("first_name", "Bryan");
            user1.set("last_name", "Jenkins");

            Assert::That(user1.save(), Equals(true));

            user1.loadBy("id", 1); // load values back up from db

            Assert::That(user1.get("first_name"), Equals("Bryan"));

        }
        catch (const database_exception &e)
        {
            cerr << "Error3: " << testdb.lastError() << endl;
            throw e;
        }
    }

    Spec(find_by)
    {
        user u1;

        u1.set("first_name", "Bob");
        u1.set("last_name", "Jenkins");

        Assert::That(u1.save(), Equals(true));

        auto res = user().findBy("first_name", "Bob");

        Assert::That(res.size(), Equals(1));

        Assert::That(res[0]->get("first_name").to_string(), Equals("Bob"));
    }

    Spec(no_column_test)
    {
        user user1;

        auto val = user1.get("missing");

        Assert::That(val, Equals(NULL));

        Assert::That(val.to_string(), Equals(""));
    }
    Spec(is_valid_test)
    {
        user user1;

        Assert::That(user1.loadBy("id", 1432123), Equals(false));

        Assert::That(user1.get("id").to_int(0), Equals(0));

    }

};

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
        setup_testdb();

    }

    static void TearDownContext()
    {
        teardown_testdb();
    }

    Spec(save_test)
    {
        try
        {
            user user1;
            user1.set("first_name", "Ryan");
            user1.set("last_name", "Jennings");

            Assert::That(user1.save(), Equals(true));

            user1.set_id(testdb->last_insert_id());

            user1.refresh(); // load values back up from db

            Assert::That(user1.get("first_name"), Equals("Ryan"));

            user1.set("first_name", "Bryan");
            user1.set("last_name", "Jenkins");

            Assert::That(user1.save(), Equals(true));

            Assert::That(user1.refresh(), Equals(true)); // load values back up from db

            Assert::That(user1.get("first_name"), Equals("Bryan"));

        }
        catch (const database_exception &e)
        {
            cerr << "Error1: " << testdb->last_error() << endl;
            throw e;
        }
    }

    Spec(find_by_id)
    {
        try
        {
            user u1;

            u1.set("first_name", "test");
            u1.set("last_name", "testing");

            Assert::That(u1.save(), Equals(true));

            auto lastId = testdb->last_insert_id();

            auto u2 = user().find_by_id(lastId);

            Assert::That(u2->is_valid(), Equals(true));

            Assert::That(u2->id(), Equals(lastId));
        }
        catch (const std::exception &e)
        {
            cerr << "Error2: " << e.what() << endl;
            throw e;
        }
    }

    Spec(find_by)
    {
        user u1;

        u1.set("first_name", "John");
        u1.set("last_name", "Jenkins");

        Assert::That(u1.save(), Equals(true));

        auto res = u1.find_by("first_name", "John");

        Assert::That(res.size() > 0, Equals(true));

        Assert::That(res[0]->get("first_name"), Equals("John"));
    }

    Spec(refresh_by)
    {
        user u1;

        u1.set("first_name", "Bender");
        u1.set("last_name", "Robot");

        Assert::That(u1.save(), Equals(true));

        user u2;

        u2.set("first_name", "Bender");

        Assert::That(u2.refresh_by("first_name"), Equals(true));

        Assert::That(u2.get("last_name"), Equals("Robot"));
    }

    Spec(no_column_test)
    {
        user user1;

        auto val = user1.get("missing");

        Assert::That(val, Equals(sql_null));
    }

    Spec(is_valid_test)
    {
        user user1(14323432);

        Assert::That(user1.refresh(), Equals(false));

    }
};

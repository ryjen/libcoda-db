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
        setup_testdb();

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
        teardown_testdb();
    }

    Spec(constructor)
    {
        modify_query query(testdb, "users");

        Assert::That(to_string(query), Equals("REPLACE INTO users DEFAULT VALUES"));

        modify_query other(query);

        Assert::That(to_string(query), Equals(to_string(other)));

        modify_query moved(std::move(query));

        Assert::That(query.is_valid(), Equals(false));

        Assert::That(to_string(moved), Equals(to_string(other)));

    }

    Spec(assignment)
    {
        modify_query query(testdb, "users");

        modify_query other(testdb, "other_users");

        other = query;

        Assert::That(to_string(query), Equals(to_string(other)));

        modify_query moved(testdb, "moved_users");

        moved = std::move(query);

        Assert::That(query.is_valid(), Equals(false));

        Assert::That(to_string(moved), Equals(to_string(other)));
    }

    Spec(modify_test)
    {
        modify_query query(testdb, "users", { "id", "first_name", "last_name" });

        query.bind(1, 1);
        query.bind(2, "blah");
        query.bind(3, "bleh");

        Assert::That(query.execute() > 0, Equals(true));

        user u1(1);

        Assert::That(u1.refresh(), Equals(true));
    };

    Spec(batch_test)
    {
        modify_query query(testdb, "users", { "id", "first_name", "last_name" });

        for (int i = 0; i < 3; i++)
        {
            char buf[100] = {0};

            query.bind(1, i + 5);

            snprintf(buf, sizeof(buf) - 1, "firstName%d", i + 1);

            query.bind(2, buf);

            snprintf(buf, sizeof(buf) - 1, "lastName%d", i + 1);

            query.bind(3, buf);

            query.execute(NULL, true);
        }

        select_query select(testdb, "users");

        int count = select.count();

        Assert::That(count, Equals(3));
    }
};
#include <igloo/igloo.h>
#include "base_record.h"
#include "delete_query.h"
#include "db.test.h"

using namespace igloo;

using namespace std;

using namespace arg3::db;

Context(delete_query_test)
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

    Spec(delete_test)
    {
        delete_query query(testdb, "users");

        query.where("first_name = ?");

        query.bind(1, "Mark");

        Assert::That(query.execute(), Equals(1));

        //Assert::That(query.last_number_of_changes(), Equals(1));
    }

    Spec(move_constructor)
    {
        delete_query query(testdb, "users");

        query.where("first_name = ?");
        query.bind(1, "Bryan");

        delete_query other(std::move(query));

        Assert::That(query.is_valid(), Equals(false));
        Assert::That(other.is_valid(), Equals(true));
    }

    Spec(copy_assign)
    {
        delete_query query(testdb, "users");

        query.where("first_name = ?");

        query.bind(1, "Bryan");

        delete_query other(testdb, "other_users");

        other = query;

        Assert::That(other.is_valid(), Equals(true));
        Assert::That(query.is_valid(), Equals(true));
        Assert::That(other.tablename(), Equals("users"));
    }

    Spec(move_assign)
    {
        delete_query query(testdb, "users");

        query.where("first_name = ?");
        query.bind(1, "Bryan");

        delete_query other(testdb, "other_users");

        other = std::move(query);

        Assert::That(query.is_valid(), Equals(false));
        Assert::That(other.is_valid(), Equals(true));
        Assert::That(other.tablename(), Equals("users"));
    }

    Spec(delete_where)
    {
        delete_query query(testdb, "users");

        where_clause where("first_name = ?");

        query.where(where);

        query.bind(1, "Bryan");

        Assert::That(query.execute(), Equals(1));
    }

    Spec(delete_batch)
    {
        delete_query query(testdb, "users");

        query.where("first_name = ?");

        query.bind(1, "Bryan");

        Assert::That(query.execute(true), Equals(1));

        query.bind(1, "Mark");

        Assert::That(query.execute(), Equals(1));
    }

    Spec(delete_record)
    {
        user u;

        u.set("first_name", "Harold");
        u.set("last_name", "Jenkins");
        u.set("dval", 439.343);

        Assert::That(u.save(), Equals(true));

        Assert::That(u.de1ete(), Equals(true));

    }
};

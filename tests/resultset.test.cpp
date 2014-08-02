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

    Spec(current_row)
    {
        select_query q(testdb, "users");

        auto rs = q.execute();

        auto i = rs.begin();

        auto row = rs.current_row();

        Assert::That(row.is_valid(), Equals(true));

        auto rowCol = row[0].to_value().to_string();

        Assert::That(rowCol.empty(), Equals(false));

        Assert::That(rowCol, Equals(i->column(0).to_value().to_string()));

    }

    Spec(reset)
    {
        select_query q(testdb, "users");

        auto rs = q.execute();

        Assert::That(rs.size(), Equals(5));

        auto i = rs.begin();

        Assert::That(i->column(1).to_string(), Equals("Bryan"));

        i++;

        Assert::That(i->column(1).to_string(), Equals("Mark"));

        rs.reset();

        if (rs.next())

            Assert::That(rs.current_row().column(1).to_string(), Equals("Bryan"));

    }

    Spec(iterator_constructors)
    {
        select_query q(testdb, "users");

        auto rs = q.execute();

        auto i = rs.begin();

        auto i2(i);

        Assert::That(i2 != rs.end(), Equals(true));
        Assert::That(i != rs.end(), Equals(true));

        Assert::That(i == i2, Equals(true));


        auto i3(std::move(i));

        Assert::That(i == rs.end(), Equals(true));

        Assert::That(i3 == i2, Equals(true));
    }

    Spec(iterator_operators)
    {
        select_query q(testdb, "users");

        auto rs = q.execute();

        auto i = rs.end();

        i++;

        Assert::That(i == rs.end(), Equals(true));

        i = rs.begin();

        ++i;

        Assert::That(i != rs.end(), Equals(true));

        auto j = i + 1;

        Assert::That(i != rs.end(), Equals(true));

        Assert::That(i < j, Equals(true));

        Assert::That(j > i, Equals(true));

        Assert::That(i <= j, Equals(true));

        Assert::That(j >= i, Equals(true));

        i += 1;

        Assert::That(i <= j, Equals(true));

        Assert::That(j >= i, Equals(true));
    }
};


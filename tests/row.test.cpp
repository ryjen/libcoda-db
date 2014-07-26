#include <igloo/igloo.h>
#include "base_record.h"
#include "row.h"
#include "db.test.h"

using namespace igloo;

using namespace std;

using namespace arg3::db;

Context(row_test)
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

    row get_first_user_row()
    {
        select_query query(testdb, "users");

        auto rs = query.execute();

        return *rs.begin();
    }

    Spec(moveable)
    {
        auto r = get_first_user_row();

        row other(std::move(r));

        Assert::That(other.is_valid(), Equals(true));

        Assert::That(other.empty(), Equals(false));
    }

    Spec(iterator)
    {
        select_query query(testdb, "users");

        auto rs = query.execute();

        auto r = *rs.begin();

        auto columns = testdb->schemas()->get("users")->column_names();

        row::const_iterator ci = r.cbegin();

        Assert::That(ci.name(), Equals(columns[0]));

        for (; ci < r.cend() - 1; ci++)
        {
            Assert::That(ci->is_valid(), Equals(true));
        }

        //Assert::That(ci.name(), Equals(columns[columns.size() - 1]));

        for (auto c : r)
        {
            Assert::That(c.is_valid(), Equals(true));
        }

        /*for (row::reverse_iterator i = r.rbegin(); i != r.rend(); i++)
        {
            Assert::That(i->is_valid(), Equals(true));
        }

        for (row::const_reverse_iterator i = r.crbegin(); i != r.crend(); i++)
        {
            Assert::That(i->is_valid(), Equals(true));
        }*/
    }
};


#include <igloo/igloo.h>
#include "base_record.h"
#include "db.test.h"
#include "sqlite3_column.h"

using namespace igloo;

using namespace std;

using namespace arg3::db;

Context(column_test)
{
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
        user u;
        u.set("first_name", "Bob");
        u.set("last_name", "Jenkins");
        u.set("dval", 123.321);

        int *data = (int *) calloc(1, sizeof(int));

        *data = 4;

        u.set("data", sql_blob(data, sizeof(int), free));

        u.save();
    }


    column get_user_column(const string & name)
    {
        select_query q(testdb, "users");

        auto rs = q.execute();

        auto row = rs.begin();

        return row->column(name);
    }

    Spec(copy_constructor)
    {
        auto col = get_user_column("first_name");

        column other(col);

        Assert::That(other.is_valid(), Equals(true));

        Assert::That(other.to_string(), Equals(col.to_string()));
    }

    Spec(move_assignment)
    {
        auto col = get_user_column("first_name");

        auto val = col.to_string();

        column other(std::move(col));

        Assert::That(col.is_valid(), Equals(false));

        Assert::That(other.is_valid(), Equals(true));

        Assert::That(other.to_string(), Equals(val));

        column last = get_user_column("last_name");

        last = std::move(other);

        Assert::That(other.is_valid(), Equals(false));
        Assert::That(last.is_valid(), Equals(true));
        Assert::That(last.to_string(), Equals(val));
    }

    Spec(to_blob)
    {
        auto col = get_user_column("data");

        Assert::That(col.to_blob().size(), Equals(4));
    }

    Spec(to_double)
    {
        auto col = get_user_column("dval");

        Assert::That(col.to_double(), Equals(123.321));

        double val = col;

        Assert::That(val, Equals(123.321));
    }

    Spec(to_int64)
    {
        auto col = get_user_column("id");

        Assert::That(col.to_int64() > 0, Equals(true));

        int64_t val = col;

        Assert::That(val > 0, Equals(true));
    }

};

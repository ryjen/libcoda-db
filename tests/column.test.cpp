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

        //Assert::That(other.to_string(), Equals(col.to_string()));
    }

    Spec(move_assignment)
    {
        auto col = get_user_column("first_name");

        //auto val = col.to_string();

        column other = get_user_column("last_name");

        other = std::move(col);

        Assert::That(col.is_valid(), Equals(false));

        Assert::That(other.is_valid(), Equals(true));

        //Assert::That(other.to_string(), Equals(val));
    }

    Spec(sqliteValueConstructor)
    {
#if TEST_SQLITE
        sqlite3_stmt *stmt;

        try
        {
            user u1(&testdb1);
            u1.set("first_name", "Bob");
            u1.set("last_name", "Jenkins");

            Assert::That(u1.save(), Equals(true));
        }
        catch (const std::exception &e)
        {
            cerr << "Error3: " << e.what() << endl;
            throw e;
        }

        if (sqlite3_prepare_v2(testdb1.rawDb(), "select * from users", -1, &stmt, NULL) != SQLITE_OK)
            throw database_exception(testdb1.last_error());

        sqlite3_step(stmt);

        sqlite3_value *value = sqlite3_column_value(stmt, 1 );

        sqlite3_column c(value);

        Assert::That(c.to_string(), Equals("Bob"));

        value = sqlite3_column_value(stmt, 0);

        c = sqlite3_column(value);

        Assert::That(c.type(), Equals(SQLITE_INTEGER));
#endif
    }
};

#include <igloo/igloo.h>
#include "base_record.h"
#include "db.test.h"

using namespace igloo;

using namespace std;

using namespace arg3::db;

Context(column_test)
{
    static void SetUpContext()
    {
        testdb.setup();

    }

    static void TearDownContext()
    {
        testdb.teardown();
    }

    Spec(defaultConstructor)
    {
        column c;

        Assert::That(c.is_valid(), Equals(false));
    }

    Spec(sqliteValueConstructor)
    {
        sqlite3_stmt *stmt;

        user u1;
        u1.set("first_name", "Bob");
        u1.set("last_name", "Jenkins");

        Assert::That(u1.save(), Equals(true));

        if (sqlite3_prepare_v2(testdb.rawDb(), "select * from users", -1, &stmt, NULL) != SQLITE_OK)
            throw database_exception(testdb.lastError());


        sqlite3_step(stmt);

        sqlite3_value * value = sqlite3_column_value(stmt, 1 );

        column c(value);

        Assert::That(c.to_string(), Equals("Bob"));

        value = sqlite3_column_value(stmt, 0);

        c = column(value);

        Assert::That(c.type(), Equals(SQLITE_INTEGER));
    }
};

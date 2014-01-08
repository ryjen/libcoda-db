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

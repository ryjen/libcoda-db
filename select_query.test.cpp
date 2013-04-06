#include <igloo/igloo.h>
#include "base_record.h"
#include "db.test.h"

using namespace igloo;

using namespace std;

using namespace arg3::db;

Context(select_query_test)
{
    static void SetUpContext()
    {
        testdb.setup();

        user user1;

        user1.set("id", 1);
        user1.set("first_name", "Bryan");
        user1.set("last_name", "Jenkins");

        user1.save();


        user user2;

        user2.set("id", 3);

        user2.set("first_name", "Bob");
        user2.set("last_name", "Smith");

        user2.save();
    }

    static void TearDownContext()
    {
        testdb.teardown();
    }

    Spec(where_test)
    {
        try
        {
            auto query = select_query(testdb, "users");

            query.where("first_name=? OR last_name=?");

            query.bind(1, "Bryan");
            query.bind(2, "Jenkins");

            auto results = query.execute();

            auto row = results.begin();

            Assert::That(row != results.end(), Equals(true));

            string lastName = row->column_value("last_name").to_string();

            Assert::That(lastName, Equals("Jenkins"));
        }
        catch(const database_exception &e)
        {
            cout << testdb.last_error() << endl;
            throw e;
        }
    }

};

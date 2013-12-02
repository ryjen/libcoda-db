#include <igloo/igloo.h>
#include "base_record.h"
#include "db.test.h"

using namespace igloo;

using namespace std;

using namespace arg3::db;


class ref_type : public base_record<ref_type>
{
public:
    ref_type() : base_record(&testdb, "users", "id") {}

    ref_type(const arg3::db::row &values) : base_record(&testdb, "users", "id", values) {}

    ref_type(long long id) : base_record(&testdb, "users", "id", id) {}

    string to_string()
    {
        ostringstream buf;

        buf << id() << ": " << get("first_name") << " " << get("last_name");

        return buf.str();
    }
};

Context(base_record_test)
{
    static void SetUpContext()
    {
        testdb.setup();

    }

    static void TearDownContext()
    {
        testdb.teardown();
    }

    Spec(save_test)
    {
        try
        {
            user user1;
            user1.set("first_name", "Ryan");
            user1.set("last_name", "Jennings");

            Assert::That(user1.save(), Equals(true));

            user1.refresh(); // load values back up from db

            Assert::That(user1.get("first_name"), Equals("Ryan"));

            user1.set("first_name", "Bryan");
            user1.set("last_name", "Jenkins");

            Assert::That(user1.save(), Equals(true));

            user1.refresh(); // load values back up from db

            Assert::That(user1.get("first_name"), Equals("Bryan"));

        }
        catch (const database_exception &e)
        {
            cerr << "Error3: " << testdb.last_error() << endl;
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

            auto u2 = user().find_by_id(1);

            Assert::That(u2->is_valid(), Equals(true));

            Assert::That(u2->id(), Equals(1));
        }
        catch (const std::exception &e)
        {
            cerr << "Error3: " << e.what() << endl;
            throw e;
        }
    }

    Spec(find_by)
    {
        user u1;

        u1.set("first_name", "Bob");
        u1.set("last_name", "Jenkins");

        Assert::That(u1.save(), Equals(true));

        auto res = user().find_by("first_name", "Bob");

        Assert::That(res.size(), Equals(1));

        Assert::That(res[0]->get("first_name"), Equals("Bob"));

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

#ifdef TEST_MYSQL

#include <bandit/bandit.h>
#include "db.test.h"
#include "mysql_column.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

shared_ptr<column_impl> get_a_column(int index)
{
    select_query query(testdb, "users");

    auto rs = query.execute();

    auto i = rs.begin();

    if (index >= i->size())
        throw database_exception("invalid column index");

    auto c = i->column(index);

    return c.impl();
}


go_bandit([]()
{

    describe("mysql column", []()
    {
        before_each([]()
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
        });

        after_each([]()
        {
            teardown_testdb();
        });

        it("is copyable", []()
        {
            auto c = get_a_column(0);

            AssertThat(c->is_valid(), IsTrue());

            auto c2 = get_a_column(1);

            AssertThat(c2->to_value() == c->to_value(), IsFalse());

            c2 = c;

            AssertThat(c2->to_value() == c->to_value(), IsTrue());
        });
    });

});

#endif

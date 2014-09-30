#include "config.h"

#undef VERSION

#if defined(TEST_MYSQL) && defined(HAVE_LIBMYSQLCLIENT)

#include <bandit/bandit.h>
#include "db.test.h"
#include "mysql_statement.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

go_bandit([]()
{
    describe("mysql statement", []()
    {
        before_each([]()
        {
            mysql_testdb.setup();

            user user1(&mysql_testdb);

            user1.set_id(1);
            user1.set("first_name", "Bryan");
            user1.set("last_name", "Jenkins");

            user1.save();

            user user2(&mysql_testdb);

            user2.set_id(3);

            user2.set("first_name", "Bob");
            user2.set("last_name", "Smith");

            user2.set("dval", 3.1456);

            user2.save();
        });

        after_each([]()
        {
            mysql_testdb.teardown();
        });
        it("is movable", []()
        {
            mysql_statement stmt(&mysql_testdb);

            mysql_statement other(std::move(stmt));

            other.prepare("select * from users");

            AssertThrows(database_exception, stmt.prepare("select * from users"));

            stmt = std::move(other);

            stmt.prepare("select * from users");

            AssertThrows(database_exception, other.prepare("select * from users"));
        });

        it("can handle an error", []()
        {
            mysql_statement stmt(&mysql_testdb);

            AssertThrows(database_exception, stmt.prepare("update users set asdfsdf='1'"));

            AssertThrows(database_exception, stmt.result());
        });
    });

});

#endif

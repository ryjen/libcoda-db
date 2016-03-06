#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#undef VERSION

#if defined(HAVE_LIBMYSQLCLIENT) && defined(TEST_MYSQL)

#include <bandit/bandit.h>
#include "db.test.h"
#include "mysql/db.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;


go_bandit([]() {

    describe("mysql database", []() {
        before_each([]() { mysql_testdb.setup(); });
        after_each([]() { mysql_testdb.teardown(); });

        it("is copyable", []() {
            mysql::db db(mysql_testdb);

            Assert::That(db.is_open(), IsTrue());

            Assert::That(db.connection_info().value, Equals(mysql_testdb.connection_info().value));

            Assert::That(mysql_testdb.is_open(), IsTrue());

            db = mysql_testdb;

            Assert::That(db.connection_info().value, Equals(mysql_testdb.connection_info().value));

            Assert::That(db.is_open(), IsTrue());

            Assert::That(mysql_testdb.is_open(), IsTrue());


        });

        it("is movable", []() {
            mysql::db other(mysql_testdb);

            other.open();

            Assert::That(other.is_open(), IsTrue());

            mysql::db db(std::move(other));

            Assert::That(other.is_open(), IsFalse());

            Assert::That(db.is_open(), IsTrue());

            other = std::move(db);

            Assert::That(db.is_open(), IsFalse());

            Assert::That(other.is_open(), IsTrue());

            other.close();
        });

        it("can handle bad parameters", []() {
            mysql::db db(uri("mysql://zzzzz:zzzzz@zzzz/zzzzz:0"));

            AssertThrows(database_exception, db.open());
        });

        it("can disable caching", []() {
            mysql::db other(mysql_testdb);

            other.open();

            other.flags(0);

            Assert::That(other.flags(), Equals(0));

            insert_query insert(&other);

            insert.into("users").columns({"first_name", "last_name"}).values("harry", "potter");

            Assert::That(insert.execute(), IsTrue());

            auto rs = other.execute("select * from users");

            Assert::That(rs.size(), Equals(1));

            select_query select(&other);

            select.from("users");

            rs = select.execute();

            Assert::That(rs.size(), Equals(1));
        });
    });

});

#endif

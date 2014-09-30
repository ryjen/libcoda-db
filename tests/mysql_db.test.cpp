#include "config.h"

#undef VERSION

#if defined(TEST_MYSQL) && defined(HAVE_LIBMYSQLCLIENT)

#include <bandit/bandit.h>
#include "db.test.h"
#include "mysql_db.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;


go_bandit([]()
{

    describe("mysql database", []()
    {
        before_each([]()
        {
            mysql_testdb.setup();
        });
        after_each([]()
        {
            mysql_testdb.teardown();
        });

        it("is copyable", []()
        {
            mysql_db db(mysql_testdb);

            Assert::That(db.is_open(), IsFalse());

            Assert::That(db.connection_string(), Equals(mysql_testdb.connection_string()));

            Assert::That(mysql_testdb.is_open(), IsTrue());

            db = mysql_testdb;

            Assert::That(db.connection_string(), Equals(mysql_testdb.connection_string()));

            Assert::That(db.is_open(), IsFalse());

            Assert::That(mysql_testdb.is_open(), IsTrue());

            db.open();

            Assert::That(db.is_open(), IsTrue());

        });

        it("is movable", []()
        {
            mysql_db other(mysql_testdb);

            other.open();

            Assert::That(other.is_open(), IsTrue());

            mysql_db db(std::move(other));

            Assert::That(other.is_open(), IsFalse());

            Assert::That(db.is_open(), IsTrue());

            other = std::move(db);

            Assert::That(db.is_open(), IsFalse());

            Assert::That(other.is_open(), IsTrue());

            other.close();
        });

        it("can handle bad parameters", []()
        {
            mysql_db db("zzzzz", "zzzzz", "zzzz", "zzzzz", 0);

            AssertThrows(database_exception, db.open());
        });
    });

});

#endif

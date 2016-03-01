#include <bandit/bandit.h>
#include "db.test.h"
#include "postgres/resultset.h"

#if defined(HAVE_LIBPQ) && defined(TEST_POSTGRES)

using namespace bandit;

using namespace std;

using namespace arg3::db;


shared_ptr<postgres::resultset> get_postgres_resultset()
{
    auto rs = postgres_testdb.execute("select * from users");

    return static_pointer_cast<postgres::resultset>(rs.impl());
}


go_bandit([]() {
    describe("a postgres resultset", []() {

        before_each([]() {
            postgres_testdb.setup();

            user user1(&postgres_testdb);

            user1.set("first_name", "Bryan");
            user1.set("last_name", "Jenkins");

            user1.save();

            user user2(&postgres_testdb);

            user2.set("first_name", "Bob");
            user2.set("last_name", "Smith");

            user2.set("dval", 3.1456);

            user2.save();
        });

        after_each([]() { postgres_testdb.teardown(); });

        it("requires valid initializer parameters", []() {
            AssertThrows(database_exception, postgres::resultset(nullptr, nullptr));
            AssertThrows(database_exception, postgres::resultset(&postgres_testdb, nullptr));
        });

        it("is movable", []() {
            auto rs = get_postgres_resultset();

            postgres::resultset other(std::move(*rs));

            Assert::That(other.is_valid(), IsTrue());

            rs = get_postgres_resultset();

            other = std::move(*rs);

            Assert::That(other.is_valid(), IsTrue());
        });
    });
});

#endif
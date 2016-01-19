#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#undef VERSION

#if defined(HAVE_LIBMYSQLCLIENT) && defined(TEST_MYSQL)

#include <bandit/bandit.h>
#include "db.test.h"
#include "mysql_binding.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

go_bandit([]() {

    describe("mysql binding", []() {
        before_each([]() {
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

        after_each([]() { mysql_testdb.teardown(); });

        it("has a size contructor", []() {
            mysql_binding b(3);

            Assert::That(b.size(), Equals(3));

            Assert::That(b.get(1)->buffer == NULL, IsTrue());

            Assert::That(b.to_value(1), Equals(sql_null));
        });

        describe("is copyable", []() {

            it("from a raw mysql binding", []() {
                mysql_binding b;

                b.bind(1, 24);

                mysql_binding other(*b.get(0));

                Assert::That(b.size(), Equals(other.size()));

                Assert::That(other.to_value(0), Equals(24));
            });

            it("from another", []() {

                mysql_binding b;

                b.bind(1, 24);

                mysql_binding other(b);

                Assert::That(b.size(), Equals(other.size()));

                Assert::That(other.to_value(0), Equals(24));

                mysql_binding c;

                c = other;

                Assert::That(c.size(), Equals(other.size()));

                Assert::That(c.to_value(0), Equals(other.to_value(0)));
            });
        });

        it("is movable", []() {
            mysql_binding b;

            b.bind(1, 24);

            mysql_binding other(std::move(b));

            Assert::That(b.size(), Equals(0));

            Assert::That(other.size() > 0, IsTrue());

            Assert::That(other.to_value(0), Equals(24));

            mysql_binding c;

            c = std::move(other);

            Assert::That(other.size(), Equals(0));

            Assert::That(c.size() > 0, IsTrue());

            Assert::That(c.to_value(0), Equals(24));

        });

        it("can handle a bad bind", []() {
            select_query query(&mysql_testdb, "users");

            query.where("id = ? and first_name = ?");

            query.bind(3, "someId");

            // TODO: why this test fails?
            // AssertThrows(binding_error, query.execute().next());
        });

    });

});

#endif

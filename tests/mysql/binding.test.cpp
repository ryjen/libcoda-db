#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#undef VERSION

#ifdef HAVE_LIBMYSQLCLIENT

#include <bandit/bandit.h>
#include "../db.test.h"
#include "mysql/binding.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

bool AreSame(double a, double b)
{
    return std::fabs(a - b) < std::numeric_limits<double>::epsilon();
}

go_bandit([]() {

    describe("mysql binding", []() {

        before_each([]() {

            setup_current_session();

            user user1;

            user1.set_id(1);
            user1.set("first_name", "Bryan");
            user1.set("last_name", "Jenkins");

            user1.save();

            user user2;

            user2.set_id(3);

            user2.set("first_name", "Bob");
            user2.set("last_name", "Smith");

            user2.set("dval", 3.1456);

            user2.save();
        });

        after_each([]() { teardown_current_session(); });

        it("has a size contructor", []() {
            mysql::binding b(3);

            Assert::That(b.size(), Equals(3));

            Assert::That(b.get(1)->buffer == NULL, IsTrue());

            Assert::That(b.to_value(1) == sql_null, IsTrue());
        });

        describe("is copyable", []() {

            it("from a raw mysql binding", []() {
                mysql::binding b;

                b.bind(1, 24);

                mysql::binding other(*b.get(0));

                Assert::That(b.size(), Equals(other.size()));

                Assert::That(other.to_value(0), Equals(24));
            });

            it("from another", []() {

                mysql::binding b;

                b.bind(1, 24);

                mysql::binding other(b);

                Assert::That(b.size(), Equals(other.size()));

                Assert::That(other.to_value(0), Equals(24));

                mysql::binding c;

                c = other;

                Assert::That(c.size(), Equals(other.size()));

                Assert::That(c.to_value(0), Equals(other.to_value(0)));
            });
        });

        it("is movable", []() {
            mysql::binding b;

            b.bind(1, 24);

            mysql::binding other(std::move(b));

            Assert::That(b.size(), Equals(0));

            Assert::That(other.size() > 0, IsTrue());

            Assert::That(other.to_value(0), Equals(24));

            mysql::binding c;

            c = std::move(other);

            Assert::That(other.size(), Equals(0));

            Assert::That(c.size() > 0, IsTrue());

            Assert::That(c.to_value(0), Equals(24));

        });

        it("can handle a bad bind", []() {
            select_query query(current_session);

            query.from("users");

            query.where("id = $1 and first_name = $2");

            query.bind(3, "someId");

            // TODO: why this test fails?
            // AssertThrows(binding_error, query.execute().next());
        });

        it("can bind a time value", []() {
            time_t tval = time(0);

            mysql::binding b;

            b.bind(1, sql_time(tval, sql_time::TIME));

            b.bind(2, sql_time(tval, sql_time::DATE));

            b.bind(3, sql_time(tval, sql_time::DATETIME));

            b.bind(4, sql_time(tval, sql_time::TIMESTAMP));

            Assert::That(b.to_value(0).to_time().to_ulong(), Equals(tval));

            Assert::That(b.to_value(1).to_time().to_ulong(), Equals(tval));

            Assert::That(b.to_value(2).to_time().to_ulong(), Equals(tval));

            Assert::That(b.to_value(3).to_time().to_ulong(), Equals(tval));
        });

        it("can bind different values", []() {

            mysql::binding b;

            b.bind(1, 1234LL);
            Assert::That(b.to_value(0), Equals(1234LL));

            b.bind(1, 1234ULL);
            Assert::That(b.to_value(0), Equals(1234ULL));


            b.bind(1, 1234U);
            Assert::That(b.to_value(0), Equals(1234U));

            b.bind(3, 1234.1234);
            Assert::That(AreSame(b.to_value(2).to_double(), 1234.1234), IsTrue());
            b.bind(3, 123.123f);
            // Assert::That(AreSame(b.to_value(2).to_float(), 123.123), IsTrue());

            b.bind(4, sql_time());

            b.bind(2, sql_null);
            Assert::That(b.to_value(1).is_null(), IsTrue());

            b.bind(2, L"test");
            Assert::That(b.to_value(1), Equals("test"));
        });

        it("can reorder and reuse indexes", []() {
            select_query select(current_session);

            select.from("users").where("first_name = $1 or last_name = $1", "Smith");

            auto results = select.execute();

            Assert::That(results.size(), Equals(1));
        });

    });

});

#endif

#include <string>

#include <bandit/bandit.h>
#include "../db.test.h"
#include "postgres/binding.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

using namespace snowhouse;

SPEC_BEGIN(postgres_binding)
{
    describe("postgres binding", []() {
        before_each([]() {
            test::setup_current_session();

            test::user user1;

            user1.set_id(1);
            user1.set("first_name", "Bryan");
            user1.set("last_name", "Jenkins");

            user1.save();

            test::user user2;

            user2.set_id(3);

            user2.set("first_name", "Bob");
            user2.set("last_name", "Smith");

            user2.set("dval", 3.1456);

            user2.save();
        });

        after_each([]() { test::teardown_current_session(); });

        it("has a size contructor", []() {
            postgres::binding b(3);

            Assert::That(b.capacity(), Equals(3));

            Assert::That(b.num_of_bindings(), Equals(0));

            Assert::That(b.to_value(1) == sql_null, IsTrue());
        });

        describe("is copyable", []() {

            it("from another", []() {

                postgres::binding b;

                b.bind(1, 24);

                Assert::That(b.to_value(0), Equals(24));

                postgres::binding other(b);

                Assert::That(b.num_of_bindings(), Equals(other.num_of_bindings()));

                Assert::That(other.to_value(0), Equals(24));

                postgres::binding c;

                c = other;

                Assert::That(c.num_of_bindings(), Equals(other.num_of_bindings()));

                Assert::That(c.to_value(0), Equals(other.to_value(0)));
            });
        });

        it("is movable", []() {
            postgres::binding b;

            b.bind(1, 24);

            postgres::binding other(std::move(b));

            Assert::That(b.num_of_bindings(), Equals(0));

            Assert::That(other.num_of_bindings() > 0, IsTrue());

            Assert::That(other.to_value(0), Equals(24));

            postgres::binding c;

            c = std::move(other);

            Assert::That(other.num_of_bindings(), Equals(0));

            Assert::That(c.num_of_bindings() > 0, IsTrue());

            Assert::That(c.to_value(0), Equals(24));

        });
    });
}
SPEC_END;

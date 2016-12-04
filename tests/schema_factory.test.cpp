#include <bandit/bandit.h>
#include "db.test.h"
#include "schema_factory.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

specification(schema_factories, []() {
    describe("schema factory", []() {
        before_each([]() { test::setup_current_session(); });

        after_each([]() { test::teardown_current_session(); });

        it("has the rule of five", []() {
            schema_factory schemas;

            schema_factory other;

            other = schemas;

            auto s = other.get(test::current_session, test::user::TABLE_NAME);

            s->init();

            Assert::That(s->is_valid(), Equals(true));

            schema_factory moved;

            moved = std::move(other);

            Assert::That(moved.get(test::current_session, test::user::TABLE_NAME) != nullptr, Equals(true));

            schema_factory a(moved);

            Assert::That(a.get(test::current_session, test::user::TABLE_NAME) != nullptr, Equals(true));

            schema_factory b(std::move(a));

            Assert::That(a.get(test::current_session, test::user::TABLE_NAME) != nullptr, Equals(true));

        });

    });
});

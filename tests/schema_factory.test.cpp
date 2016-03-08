#include <bandit/bandit.h>
#include "schema_factory.h"
#include "db.test.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

go_bandit([]() {

    describe("schema factory", []() {
        before_each([]() { setup_current_session(); });

        after_each([]() { teardown_current_session(); });

        it("has the rule of five", []() {
            schema_factory schemas;

            schema_factory other;

            other = schemas;

            auto s = other.get(current_session, "users");

            s->init();

            Assert::That(s->is_valid(), Equals(true));

            schema_factory moved;

            moved = std::move(other);

            Assert::That(moved.get(current_session, "users") != nullptr, Equals(true));

            schema_factory a(moved);

            Assert::That(a.get(current_session, "users") != nullptr, Equals(true));

            schema_factory b(std::move(a));

            Assert::That(a.get(current_session, "users") != nullptr, Equals(true));

        });

    });


});

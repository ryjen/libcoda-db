#include <string>

#include <bandit/bandit.h>
#include "db.test.h"
#include "schema.h"

using namespace bandit;

using namespace std;

using namespace coda::db;

using namespace snowhouse;

specification(schemas, []() {
    describe("schema", []() {
        before_each([]() { test::setup_current_session(); });

        after_each([]() { test::teardown_current_session(); });


        it("has primary keys", []() {
            test::user u;

            auto keys = u.schema()->primary_keys();

            Assert::That(keys.size(), Equals(1));

            Assert::That(keys[0], Equals("id"));
        });

        it("has operators", []() {
            schema s(test::current_session, "users");

            s.init();

            schema other(std::move(s));

            Assert::That(s.is_valid(), Equals(false));

            Assert::That(other.is_valid(), Equals(true));

            schema copy(test::current_session, "other_users");

            copy = other;

            Assert::That(copy.table_name(), Equals(other.table_name()));

            schema moved(test::current_session, "moved_users");

            moved = std::move(other);

            Assert::That(moved.is_valid(), Equals(true));

            Assert::That(other.is_valid(), Equals(false));
        });

        it("has columns", []() {
            schema s(test::current_session, "users");

            s.init();

            auto cd = s[0];

            ostringstream os;

            os << cd;

            Assert::That(os.str(), !Equals(""));
        });

    });
});

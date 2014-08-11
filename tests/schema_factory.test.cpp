#include <bandit/bandit.h>
#include "schema_factory.h"
#include "db.test.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

go_bandit([]()
{

    describe("schema factory", []()
    {
        before_each([]()
        {
            setup_testdb();
        });

        after_each([]()
        {
            teardown_testdb();
        });

        it("has the rule of five", []()
        {
            auto schemas = testdb->schemas();

            schema_factory other((sqldb *) 1);

            other = *schemas;

            auto s = other.get("users");

            s->init();

            Assert::That(s->is_valid(), Equals(true));

            schema_factory moved((sqldb *) 1);

            moved = std::move(other);

            Assert::That(moved.get("users") != nullptr, Equals(true));

            schema_factory a(moved);

            Assert::That(a.get("users") != nullptr, Equals(true));

            schema_factory b(std::move(a));

            Assert::That(a.get("users") != nullptr, Equals(true));

        });

    });


});


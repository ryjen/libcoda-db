#include <string>

#include <bandit/bandit.h>
#include "db.test.h"
#include "insert_query.h"
#include "util.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

using namespace snowhouse;

specification(modifying, []() {
    describe("modify query", []() {
        before_each([]() {
            test::setup_current_session();

            test::user user1;

            user1.set("first_name", "Bryan");
            user1.set("last_name", "Jenkins");

            user1.save();

            test::user user2;

            user2.set("first_name", "Bob");
            user2.set("last_name", "Smith");

            user2.save();
        });

        after_each([]() { test::teardown_current_session(); });

        it("can be constructed", []() {
            insert_query query(test::current_session, "users");

            if (test::current_session->has_feature(session::FEATURE_RETURNING)) {
                Assert::That(query.to_sql(), Equals("INSERT INTO users() VALUES() RETURNING id;"));
            } else {
                Assert::That(query.to_sql(), Equals("INSERT INTO users() VALUES();"));
            }
            insert_query other(query);

            Assert::That(query.to_sql(), Equals(other.to_sql()));

            insert_query moved(std::move(query));

            Assert::That(query.is_valid(), Equals(false));

            Assert::That(moved.to_sql(), Equals(other.to_sql()));

        });

        it("can modify", []() {
            insert_query insert(test::current_session);

            vector<sql_value> values = {"blah", "bleh"};

            insert.into(test::user::TABLE_NAME).columns("first_name", "last_name").values(values);

            Assert::That(insert.execute() > 0, Equals(true));

            test::user u1(insert.last_insert_id());

            Assert::That(u1.get("first_name"), Equals("blah"));
            Assert::That(u1.get("last_name"), Equals("bleh"));
        });

        it("can modify with a set of values", []() {
            insert_query insert(test::current_session);

            insert.into(test::user::TABLE_NAME)
                .columns({"first_name", "last_name"})
                .values(std::vector<sql_value>({"blah", "bleh"}));

            Assert::That(insert.execute() > 0, Equals(true));

            test::user u1(insert.last_insert_id());

            Assert::That(u1.get("first_name"), Equals("blah"));
            Assert::That(u1.get("last_name"), Equals("bleh"));
        });

        it("can be batch executed", []() {
            insert_query query(test::current_session, "users", {"first_name", "last_name"});

            for (int i = 0; i < 3; i++) {
                char buf[100] = {0};

                snprintf(buf, sizeof(buf) - 1, "firstName%d", i + 1);

                query.value(string(buf));

                snprintf(buf, sizeof(buf) - 1, "lastName%d", i + 1);

                query.value(string(buf));

                Assert::That(query.execute(), Equals(1));
            }

            select_query select(test::current_session);

            int count = select.from(test::user::TABLE_NAME).count();

            Assert::That(count, Equals(5));
        });

    });
});

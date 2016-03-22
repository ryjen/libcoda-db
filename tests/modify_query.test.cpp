#include <bandit/bandit.h>
#include "modify_query.h"
#include "db.test.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

go_bandit([]() {

    describe("modify query", []() {
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

            user2.save();
        });

        after_each([]() { teardown_current_session(); });

        it("can be constructed", []() {
            insert_query query(current_session, "users");

#if TEST_POSTGRES
            Assert::That(query.to_string(), Equals("INSERT INTO users() VALUES() RETURNING id;"));
#else
            Assert::That(query.to_string(), Equals("INSERT INTO users() VALUES();"));
#endif
            insert_query other(query);

            Assert::That(query.to_string(), Equals(other.to_string()));

            insert_query moved(std::move(query));

            Assert::That(query.is_valid(), Equals(false));

            Assert::That(moved.to_string(), Equals(other.to_string()));

        });

        it("can modify", []() {
            insert_query insert(current_session);

            int id = rand() % 5000;

            insert.into("users").columns("id", "first_name", "last_name").values(id, "blah", "bleh");

            Assert::That(insert.execute() > 0, Equals(true));

            user u1(id);

            Assert::That(u1.get("first_name"), Equals("blah"));
            Assert::That(u1.get("last_name"), Equals("bleh"));
        });

        it("can modify with a set of values", []() {
            insert_query insert(current_session);

            int id = rand() % 5000;

            insert.into("users").columns({"id", "first_name", "last_name"}).values({id, "blah", "bleh"});

            Assert::That(insert.execute() > 0, Equals(true));

            user u1(id);

            Assert::That(u1.get("first_name"), Equals("blah"));
            Assert::That(u1.get("last_name"), Equals("bleh"));
        });

        it("can be batch executed", []() {
            insert_query query(current_session, "users", {"id", "first_name", "last_name"});

            query.flags(modify_query::Batch);

            for (int i = 0; i < 3; i++) {
                char buf[100] = {0};

                query.bind(1, i + 5);

                snprintf(buf, sizeof(buf) - 1, "firstName%d", i + 1);

                query.bind(2, buf);

                snprintf(buf, sizeof(buf) - 1, "lastName%d", i + 1);

                query.bind(3, buf);

                Assert::That(query.execute(), Equals(1));
            }

            select_query select(current_session);

            int count = select.from("users").count();

            Assert::That(count, Equals(5));
        });

    });

});

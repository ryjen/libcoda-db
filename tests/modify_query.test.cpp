#include <bandit/bandit.h>
#include "base_record.h"
#include "modify_query.h"
#include "db.test.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

go_bandit([]() {

    describe("modify query", []() {
        before_each([]() {
            setup_testdb();

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

        after_each([]() { teardown_testdb(); });

        it("can be constructed", []() {
            insert_query query(testdb, "users");

            Assert::That(query.to_string(), Equals("INSERT INTO users DEFAULT VALUES;"));

            insert_query other(query);

            Assert::That(query.to_string(), Equals(other.to_string()));

            insert_query moved(std::move(query));

            Assert::That(query.is_valid(), Equals(false));

            Assert::That(moved.to_string(), Equals(other.to_string()));

        });

        it("can be assigned", []() {
            update_query query(testdb, "users");

            update_query other(testdb, "other_users");

            other = query;

            Assert::That(query.to_string(), Equals(other.to_string()));

            update_query moved(testdb, "moved_users");

            moved = std::move(query);

            Assert::That(query.is_valid(), Equals(false));

            Assert::That(moved.to_string(), Equals(other.to_string()));
        });

        it("can modify", []() {
            insert_query query(testdb, "users", {"id", "first_name", "last_name"});

            int id = rand() % 5000;

            query.bind(1, id);
            query.bind(2, "blah");
            query.bind(3, "bleh");

            Assert::That(query.execute() > 0, Equals(true));

            user u1(id);

            Assert::That(u1.refresh(), Equals(true));
        });

        it("can be batch executed", []() {
            insert_query query(testdb, "users", {"id", "first_name", "last_name"});

            query.set_flags(modify_query::Batch);

            for (int i = 0; i < 3; i++) {
                char buf[100] = {0};

                query.bind(1, i + 5);

                snprintf(buf, sizeof(buf) - 1, "firstName%d", i + 1);

                query.bind(2, buf);

                snprintf(buf, sizeof(buf) - 1, "lastName%d", i + 1);

                query.bind(3, buf);

                Assert::That(query.execute(), Equals(1));
            }

            select_query select(testdb, "users");

            int count = select.count();

            Assert::That(count, Equals(5));
        });

    });

});

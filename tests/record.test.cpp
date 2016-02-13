#include <bandit/bandit.h>
#include "db.test.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

go_bandit([]() {

    describe("a user record", []() {
        before_each([]() { setup_testdb(); });

        after_each([]() { teardown_testdb(); });

        it("should save", []() {
            try {
                user user1;
                user1.set("first_name", "Ryan");
                user1.set("last_name", "Jennings");

                // long long insertId = 0;
                Assert::That(user1.save(), IsTrue());

                Assert::That(user1.refresh(), IsTrue());  // load values back up from db

                Assert::That(user1.get("first_name"), Equals("Ryan"));

                user1.set("first_name", "Bryan");
                user1.set("last_name", "Jenkins");

                Assert::That(user1.save(), IsTrue());

                Assert::That(user1.refresh(), IsTrue());  // load values back up from db

                Assert::That(user1.get("first_name"), Equals("Bryan"));
            } catch (const database_exception& e) {
                cerr << "Error1: " << testdb->last_error() << endl;
                throw e;
            }
        });

        it("should find by id", []() {
            try {
                user u1;

                u1.set("first_name", "test");
                u1.set("last_name", "testing");

                Assert::That(u1.save(), IsTrue());

                auto lastId = testdb->last_insert_id();

                auto u2 = user().find_by_id(lastId);

                Assert::That(u2->is_valid(), IsTrue());

                Assert::That(u2->id(), Equals(lastId));
            } catch (const std::exception& e) {
                cerr << "Error2: " << e.what() << endl;
                throw e;
            }
        });

        it("can find all", []() {
            user u1;
            u1.set("first_name", "Barry");
            u1.set("last_name", "White");

            Assert::That(u1.save(), IsTrue());

            user u2;

            u2.set("first_name", "Bono");
            u2.set("last_name", "Bono");

            Assert::That(u2.save(), IsTrue());

            auto results = user().find_all();

            Assert::That(results.size(), Equals(2));
        });

        it("can find by a column", []() {
            user u1;

            u1.set("first_name", "John");
            u1.set("last_name", "Jenkins");

            Assert::That(u1.save(), IsTrue());

            auto res = u1.find_by("first_name", "John");

            Assert::That(res.size() > 0, IsTrue());

            Assert::That(res[0]->get("first_name"), Equals("John"));
        });

        it("can refresh by a column", []() {
            user u1;

            u1.set("first_name", "Bender");
            u1.set("last_name", "Robot");

            Assert::That(u1.save(), IsTrue());

            user u2;

            u2.set("first_name", "Bender");

            Assert::That(u2.refresh_by("first_name"), IsTrue());

            Assert::That(u2.get("last_name"), Equals("Robot"));
        });

        it("can have no column", []() {
            user user1;

            auto val = user1.get("missing");

            Assert::That(val == nullptr, IsTrue());
        });

        it("cannnot refresh invalid", []() {
            user user1(14323432);

            Assert::That(user1.refresh(), Equals(false));

        });
    });
});

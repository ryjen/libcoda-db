#include <bandit/bandit.h>
#include "modify_query.h"
#include "db.test.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

go_bandit([]() {

    describe("a delete query", []() {

        before_each([]() {
            user user1;
            user user2;

            try {
                setup_testdb();

                user1.set("first_name", "Bryan");
                user1.set("last_name", "Jenkins");

                user1.save();

                user2.set("first_name", "Mark");
                user2.set("last_name", "Smith");

                user2.save();

            } catch (const std::exception& e) {
                cerr << "setup error: " << e.what() << endl;
                throw e;
            }
        });

        after_each([]() { teardown_testdb(); });

        it("can delete", []() {
            delete_query query(testdb);

            query.from("users").where("first_name = $1", "Mark");

            AssertThat(query.execute(), Equals(1));

            AssertThat(query.last_number_of_changes(), Equals(1));
        });

        it("is copyable by constructor", []() {
            delete_query query(testdb);

            query.from("users").where("first_name = $1", "Mark");

            delete_query other(query);

            AssertThat(other.to_string(), Equals(query.to_string()));
        });

        it("is movable by constructor", []() {
            delete_query query(testdb);

            query.from("users").where("first_name = $1", "Bryan");

            delete_query other(std::move(query));

            AssertThat(query.is_valid(), IsFalse());
            AssertThat(other.is_valid(), IsTrue());
        });

        it("is copyable from assignment", []() {
            delete_query query(testdb);

            query.from("users").where("first_name = $1", "Bryan");

            delete_query other(testdb, "other_users");

            other = query;

            AssertThat(other.is_valid(), IsTrue());
            AssertThat(query.is_valid(), IsTrue());
            AssertThat(other.from(), Equals("users"));
        });

        it("is movable from assignment", []() {
            delete_query query(testdb);

            query.from("users").where("first_name = $1", "Bryan");

            delete_query other(testdb, "other_users");

            other = std::move(query);

            AssertThat(query.is_valid(), IsFalse());
            AssertThat(other.is_valid(), IsTrue());
            AssertThat(other.from(), Equals("users"));
        });

        it("can delete from where clause", []() {
            delete_query query(testdb, "users");

            where_clause where("first_name = $1");

            query.where(where);

            query.bind(1, "Mark");

            AssertThat(query.execute(), Equals(1));
        });

        it("can be batch executed", []() {
            delete_query query(testdb, "users");

            query.where("first_name = $1", "Bryan");

            query.set_flags(modify_query::Batch);

            AssertThat(query.execute(), Equals(1));

            query.bind(1, "Mark");

            AssertThat(query.execute(), Equals(1));
        });

        it("can be deleted from a record", []() {

            auto u = user().find_one("first_name", "Bryan");

            AssertThat(u != nullptr, Equals(true));
            AssertThat(u->de1ete(), IsTrue());

        });

    });

});

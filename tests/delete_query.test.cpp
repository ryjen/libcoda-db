#include <bandit/bandit.h>
#include "db.test.h"
#include "modify_query.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

SPEC_BEGIN(delete_query)
{
    describe("a delete query", []() {

        before_each([]() {
            test::user user1;
            test::user user2;

            try {
                test::setup_current_session();

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

        after_each([]() { test::teardown_current_session(); });

        it("can delete", []() {
            delete_query query(test::current_session);

            query.from(test::user::TABLE_NAME).where(op::equals("first_name", "Mark"));

            AssertThat(query.execute(), Equals(1));

            AssertThat(query.last_number_of_changes(), Equals(1));
        });

        it("is copyable by constructor", []() {
            delete_query query(test::current_session);

            query.from(test::user::TABLE_NAME).where(op::equals("first_name", "Mark"));

            delete_query other(query);

            AssertThat(other.to_string(), Equals(query.to_string()));
        });

        it("is movable by constructor", []() {
            delete_query query(test::current_session);

            query.from(test::user::TABLE_NAME).where(op::equals("first_name", "Bryan"));

            delete_query other(std::move(query));

            AssertThat(query.is_valid(), IsFalse());
            AssertThat(other.is_valid(), IsTrue());
        });

        it("is copyable from assignment", []() {
            delete_query query(test::current_session);

            query.from(test::user::TABLE_NAME).where(op::equals("first_name", "Bryan"));

            delete_query other(test::current_session, "other_users");

            other = query;

            AssertThat(other.is_valid(), IsTrue());
            AssertThat(query.is_valid(), IsTrue());
            AssertThat(other.from(), Equals("users"));
        });

        it("is movable from assignment", []() {
            delete_query query(test::current_session);

            query.from(test::user::TABLE_NAME).where(op::equals("first_name", "Bryan"));

            delete_query other(test::current_session, "other_users");

            other = std::move(query);

            AssertThat(query.is_valid(), IsFalse());
            AssertThat(other.is_valid(), IsTrue());
            AssertThat(other.from(), Equals("users"));
        });

        it("can delete from where clause", []() {
            delete_query query(test::current_session, "users");

            where_clause where("first_name = 'Mark'");

            query.where(where);

            AssertThat(query.execute(), Equals(1));
        });

        it("can be batch executed", []() {
            delete_query query(test::current_session, "users");

            query.where(op::equals("first_name", "Bryan"));

            AssertThat(query.execute(), Equals(1));

            query.where(op::equals("first_name", "Mark"));

            AssertThat(query.execute(), Equals(1));
        });

        it("can be deleted from a record", []() {

            auto u = test::user().find_one("first_name", "Bryan");

            AssertThat(u != nullptr, Equals(true));
            AssertThat(u->de1ete(), IsTrue());

        });

    });
}
SPEC_END;

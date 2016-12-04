#include <bandit/bandit.h>
#include "db.test.h"
#include "row.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

row get_first_user_row()
{
    select_query query(test::current_session);

    auto rs = query.from("users").execute();

    auto i = rs.begin();

    if (i == rs.end()) throw database_exception("No rows in users table");

    return *i;
}

specification(rows, []() {
    describe("a row", []() {

        before_each([&]() {
            test::setup_current_session();

            test::user user1;
            test::user user2;

            user1.set("first_name", "Bryan");
            user1.set("last_name", "Jenkins");

            user1.save();

            user2.set("first_name", "Mark");
            user2.set("last_name", "Smith");

            user2.save();
        });

        after_each([]() { test::teardown_current_session(); });

        it("can be movable", []() {
            auto r = get_first_user_row();

            row other(std::move(r));

            AssertThat(other.is_valid(), IsTrue());

            AssertThat(other.empty(), IsFalse());
        });

        it("can be copied", []() {
            select_query q(test::current_session);

            auto rs = q.from(test::user::TABLE_NAME).execute();

            auto i = rs.begin();

            row other;

            other = *i;

            AssertThat(other.empty(), IsFalse());

            AssertThat(other.size(), Equals(i->size()));

            AssertThat(other.column_name(0), Equals("id"));
        });

        it("can use for each", []() {
            select_query query(test::current_session);

            auto rs = query.from(test::user::TABLE_NAME).execute();

            auto r = rs.begin();

            r->for_each([](const rj::db::column& c) { Assert::That(c.is_valid(), IsTrue()); });
        });

        it("has an iterator", []() {

            auto schema = test::current_session->get_schema(test::user::TABLE_NAME);

            if (!schema->is_valid()) {
                schema->init();
            }

            auto columns = schema->column_names();

            AssertThat(columns.size() > 0, IsTrue());

            select_query query(test::current_session, columns);

            auto rs = query.from(test::user::TABLE_NAME).execute();

            auto r = *rs.begin();

            row::const_iterator ci = r.cbegin();

            AssertThat(ci.name(), Equals(columns[0]));

            for (; ci < r.cend(); ci++) {
                AssertThat(ci->is_valid(), IsTrue());
            }

            // AssertThat(ci.name(), Equals(columns[columns.size() - 1]));

            for (auto& c : r) {
                AssertThat(c.is_valid(), IsTrue());
            }

        });
    });
});

#include <bandit/bandit.h>
#include "base_record.h"
#include "row.h"
#include "db.test.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

row get_first_user_row()
{
    select_query query(testdb, "users");

    auto rs = query.execute();

    auto i = rs.begin();

    if (i == rs.end()) throw database_exception("No rows in users table");

    return *i;
}

go_bandit([]() {

    describe("a row", []() {
        user user1;
        user user2;

        before_each([]() { setup_testdb(); });

        before_each([&]() {
            user1.set("first_name", "Bryan");
            user1.set("last_name", "Jenkins");

            user1.save();

            user2.set("first_name", "Mark");
            user2.set("last_name", "Smith");

            user2.save();
        });

        after_each([&]() {
            user1.de1ete();
            user2.de1ete();
        });

        after_each([]() { teardown_testdb(); });

        it("can be movable", []() {
            auto r = get_first_user_row();

            row other(std::move(r));

            AssertThat(other.is_valid(), IsTrue());

            AssertThat(other.empty(), IsFalse());
        });

        it("can be copied", []() {
            select_query q(testdb, "users");

            auto rs = q.execute();

            auto i = rs.begin() + 2;

            row other = get_first_user_row();

            other = *i;

            AssertThat(other.empty(), IsFalse());

            AssertThat(other.size(), Equals(i->size()));

            AssertThat(other.column_name(0), Equals("id"));
        });

        it("can use for each", []() {
            select_query query(testdb, "users");

            auto rs = query.execute();

            auto r = rs.begin();

            r->for_each([](const arg3::db::column& c) { Assert::That(c.is_valid(), IsTrue()); });
        });

        it("has an iterator", []() {

            auto schema = testdb->schemas()->get("users");

            if (!schema->is_valid()) schema->init();

            select_query query(testdb, "users");

            auto rs = query.execute();

            auto r = *rs.begin();

            auto columns = schema->column_names();

            AssertThat(columns.size() > 0, IsTrue());

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

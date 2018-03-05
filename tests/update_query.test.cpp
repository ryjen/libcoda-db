#include <string>

#include <bandit/bandit.h>
#include "db.test.h"
#include "update_query.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

using namespace snowhouse;

specification(updates, []() {
    describe("update clause", []() {

        it("can be constructed", []() {
            update_query query(test::current_session, test::user::TABLE_NAME, {"id"});

            Assert::That(query.to_sql(),
                         Equals("UPDATE users SET id=" + test::current_session->impl()->bind_param(1) + ";"));

            update_query other(query);

            Assert::That(query.to_sql(), Equals(other.to_sql()));

            update_query moved(std::move(query));

            Assert::That(query.is_valid(), Equals(false));

            Assert::That(moved.to_sql(), Equals(other.to_sql()));

        });
        it("can be assigned", []() {
            update_query query(test::current_session, test::user::TABLE_NAME);

            update_query other(test::current_session, "other_users");

            other = query;

            Assert::That(query.to_sql(), Equals(other.to_sql()));

            update_query moved(test::current_session, "moved_users");

            moved = std::move(query);

            Assert::That(query.is_valid(), Equals(false));

            Assert::That(moved.to_sql(), Equals(other.to_sql()));
        });

        it("can set the table", []() {
            update_query query(test::current_session);

            query.table(test::user::TABLE_NAME);

            Assert::That(query.table(), Equals(test::user::TABLE_NAME));
        });

        it("can set the columns", []() {
            update_query query(test::current_session);

            query.columns("id", "first_name");

            Assert::That(query.columns().size(), Equals(2));
        });

    });
});

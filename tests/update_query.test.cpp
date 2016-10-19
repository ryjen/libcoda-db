#include <bandit/bandit.h>
#include "db.test.h"
#include "update_query.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

go_bandit([]() {

    describe("update clause", []() {

        it("can be constructed", []() {
            update_query query(current_session, "users", {"id"});

            Assert::That(query.to_string(), Equals("UPDATE users SET id=" + current_session->impl()->bind_param(1) + ";"));

            update_query other(query);

            Assert::That(query.to_string(), Equals(other.to_string()));

            update_query moved(std::move(query));

            Assert::That(query.is_valid(), Equals(false));

            Assert::That(moved.to_string(), Equals(other.to_string()));

        });
        it("can be assigned", []() {
            update_query query(current_session, "users");

            update_query other(current_session, "other_users");

            other = query;

            Assert::That(query.to_string(), Equals(other.to_string()));

            update_query moved(current_session, "moved_users");

            moved = std::move(query);

            Assert::That(query.is_valid(), Equals(false));

            Assert::That(moved.to_string(), Equals(other.to_string()));
        });

        it("can set the table", []() {
            update_query query(current_session);

            query.table("users");

            Assert::That(query.table(), Equals("users"));
        });

        it("can set the columns", []() {
            update_query query(current_session);

            query.columns("id", "first_name");

            Assert::That(query.columns().size(), Equals(2));
        });

    });

});

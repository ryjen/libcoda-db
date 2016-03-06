#include <bandit/bandit.h>
#include "update_query.h"
#include "db.test.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

go_bandit([]() {

    describe("update clause", []() {

        it("can be constructed", []() {
            update_query query(testdb, "users", {"id"});

            Assert::That(query.to_string(), Equals("UPDATE users SET id = $1;"));

            update_query other(query);

            Assert::That(query.to_string(), Equals(other.to_string()));

            update_query moved(std::move(query));

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

        it("can set the table", []() {
            update_query query(testdb);

            query.table("users");

            Assert::That(query.table(), Equals("users"));
        });

        it("can set the columns", []() {
            update_query query(testdb);

            query.columns({"id", "first_name"});

            Assert::That(query.columns().size(), Equals(2));
        });

        it("can set the where clause", []() {
            update_query query(testdb);

            where_clause clause("a = b");

            query.where(clause);

            Assert::That(query.where().to_string(), Equals("a = b"));
        });
    });

});
#include <bandit/bandit.h>
#include "db.test.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

resultset get_results()
{
    select_query query(testdb, "users");

    return query.execute();
}

go_bandit([]() {

    describe("select query", []() {
        before_each([]() {
            setup_testdb();

            user user1;

            user1.set("first_name", "Bryan");
            user1.set("last_name", "Jenkins");

            user1.save();

            user user2;

            user2.set("first_name", "Bob");
            user2.set("last_name", "Smith");

            user2.save();
        });

        after_each([]() { teardown_testdb(); });

        it("has database info", []() {
            select_query query(testdb, "users");

            Assert::That(query.db(), Equals(testdb));

            Assert::That(query.table_name(), Equals("users"));
        });

        it("can be constructed with a schema", []() {
            schema_factory factory(testdb);

            select_query query(factory.get("users"));

            Assert::That(query.table_name(), Equals("users"));
        });

        it("can be copied and moved", []() {
            select_query query(testdb, "users", {"id"});

            select_query other(query);

            Assert::That(query.to_string(), Equals(other.to_string()));

            select_query moved(std::move(query));

            Assert::That(query.is_valid(), Equals(false));

            Assert::That(moved.to_string(), Equals(other.to_string()));

            select_query other2(testdb, "other_users");

            other2 = other;

            Assert::That(other.to_string(), Equals(other2.to_string()));

            select_query moved2(testdb, "moved_users");

            moved2 = std::move(other2);

            Assert::That(other2.is_valid(), Equals(false));

            Assert::That(moved2.to_string(), Equals(other.to_string()));
        });

        it("has validity", []() {
            auto rs = get_results();

            for (auto row : rs) {
                Assert::That(row.is_valid(), Equals(true));
            }
            Assert::That(rs.is_valid(), Equals(true));
        });

        it("can use callbacks", []() {
            select_query query(testdb, "users");

            query.execute([](const resultset& rs) {
                AssertThat(rs.is_valid(), IsTrue());

                rs.for_each([](const row& r) {
                    AssertThat(r.is_valid(), IsTrue());

                    r.for_each([](const column& c) { AssertThat(c.is_valid(), IsTrue()); });
                });
            });
        });

        it("can be used with a where clause", []() {
            auto query = select_query(testdb, "users");

            try {
                query.where("first_name=$1 OR last_name=$2");

                query.bind(1, "Bryan");
                query.bind(2, "Jenkins");

                auto results = query.execute();

                auto row = results.begin();

                Assert::That(row != results.end(), Equals(true));

                string lastName = row->column("last_name").to_value();

                Assert::That(lastName, Equals("Jenkins"));

                query.reset();

                where_clause w("last_name = $1");

                query.where(w);

                query.bind(1, "Smith");

                results = query.execute();

                row = results.begin();

                Assert::That(row != results.end(), Equals(true));

                lastName = row->column("last_name").to_value().to_string();

                Assert::That(lastName, Equals("Smith"));
            } catch (const database_exception& e) {
                cout << query.last_error() << endl;
                throw e;
            }
        });

        it("can execute scalar", []() {
            vector<string> columns = {"first_name"};

            auto query = select_query(testdb, "users", columns);

            query.where("first_name=$1");

            query.bind(1, "Bryan");

            string value = query.execute_scalar<string>();

            Assert::That(value, Equals("Bryan"));
        });

        it("can be binded", []() {
            select_query query(testdb, "users");

            // sneaky, bind first, should be able to handle it
            query.bind(1, "Bryan");

            query.bind(2, "Jenkins");

            query.where("first_name=$1 and last_name=$2");

            query.execute([](const resultset& rs) {
                Assert::That(rs.is_valid(), Equals(true));

                auto u = rs.begin()->column("first_name");

                Assert::That(u.to_value(), Equals("Bryan"));
            });

        });

    });

});

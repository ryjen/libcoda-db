#include <bandit/bandit.h>
#include "db.test.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

resultset get_results()
{
    select_query query(test::current_session);

    return query.from(test::user::TABLE_NAME).execute();
}

SPEC_BEGIN(select_query)
{
    describe("select query", []() {
        before_each([]() {
            test::setup_current_session();

            test::user user1;

            user1.set("first_name", "Bryan");
            user1.set("last_name", "Jenkins");

            user1.save();

            test::user user2;

            user2.set("first_name", "Bob");
            user2.set("last_name", "Smith");

            user2.save();
        });

        after_each([]() { test::teardown_current_session(); });

        it("has database info", []() {
            select_query query(test::current_session);

            query.from(test::user::TABLE_NAME);

            Assert::That(query.get_session(), Equals(test::current_session));

            Assert::That(query.from(), Equals(test::user::TABLE_NAME));
        });

        it("can be constructed with a schema", []() {
            schema_factory factory;

            select_query query(factory.get(test::current_session, test::user::TABLE_NAME));

            Assert::That(query.from(), Equals(test::user::TABLE_NAME));
        });

        it("can be copied and moved", []() {
            select_query query(test::current_session, {"id"});

            query.from(test::user::TABLE_NAME);

            select_query other(query);

            Assert::That(query.to_string(), Equals(other.to_string()));

            select_query moved(std::move(query));

            Assert::That(query.is_valid(), Equals(false));

            Assert::That(moved.to_string(), Equals(other.to_string()));

            select_query other2(test::current_session);

            other2.from("other_users");

            other2 = other;

            Assert::That(other.to_string(), Equals(other2.to_string()));

            select_query moved2(test::current_session);

            moved2.from("moved_users");

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
            select_query query(test::current_session);

            query.from(test::user::TABLE_NAME);

            query.execute([](const resultset& rs) {
                AssertThat(rs.is_valid(), IsTrue());

                rs.for_each([](const row& r) {
                    AssertThat(r.is_valid(), IsTrue());

                    r.for_each([](const column& c) { AssertThat(c.is_valid(), IsTrue()); });
                });
            });
        });

        it("can be used with a where clause", []() {
            auto query = select_query(test::current_session);

            query.columns("first_name", "last_name").from(test::user::TABLE_NAME);

            try {
                query.where(op::equals("first_name", "Bryan")) || op::equals("last_name", "Jenkins");

                auto results = query.execute();

                auto row = results.begin();

                Assert::That(row != results.end(), Equals(true));

                string lastName = row->column("last_name").value();

                Assert::That(lastName, Equals("Jenkins"));

                query.reset();

                query.where(op::equals("last_name", "Smith"));

                results = query.execute();

                row = results.begin();

                Assert::That(row != results.end(), Equals(true));

                lastName = row->column("last_name").value().to_string();

                Assert::That(lastName, Equals("Smith"));
            } catch (const database_exception& e) {
                cout << query.last_error() << endl;
                throw e;
            }
        });

        it("can execute scalar", []() {
            auto query = select_query(test::current_session);

            query.columns("first_name").from(test::user::TABLE_NAME);

            query.where(op::equals("first_name", "Bryan"));

            string value = query.execute_scalar<string>();

            Assert::That(value, Equals("Bryan"));
        });

        it("can union another", []() {
            select_query query(test::current_session);

            query.from(test::user::TABLE_NAME);

            select_query other(test::current_session);

            other.from("user_settings");

            query.union_with(other);

            Assert::That(query.to_string(), Equals("SELECT * FROM users UNION SELECT * FROM user_settings;"));

            query.union_with(other, union_op::all);

            Assert::That(query.to_string(), Equals("SELECT * FROM users UNION ALL SELECT * FROM user_settings;"));
        });

#ifdef ENHANCED_PARAMETER_MAPPING

        it("can use named parameters", []() {

            select_query query(test::current_session);

            query.from(test::user::TABLE_NAME);

            query.where("first_name = @name OR last_name = @name");

            query.bind("@name", "Bryan");

            auto callback = [](const resultset& rs) {
                rs.for_each([](const row& row) {
                    Assert::That(row.column("first_name").value().to_string() == "Bryan" || row.column("last_name").value().to_string() == "Bryan");
                });

            };

            if (current_session->has_feature(session::FEATURE_NAMED_PARAMS)) {
                query.execute(callback);
            } else {
                AssertThrows(database_exception, query.execute());
            }
        });

        it("can use different parameter types", []() {
            select_query query(test::current_session);

            query.from(test::user::TABLE_NAME);

            query.where("(first_name = ? and last_name = ?) or (first_name = ? or last_name = ?) or last_name = @lname");
            query.bind(1, "Bob");
            query.bind(2, "Smith");
            query.bind(3, "Bryan");
            query.bind(4, "Smith");
            query.bind("@lname", "Jenkins");
            auto rs = query.execute();

            Assert::That(rs.size() > 0, IsTrue());

            query.reset();

            query.where("(first_name = $1 and last_name = $2) or (first_name = $3 or last_name = $2) or last_name = @lname");

            query.bind(1, "Bob");
            query.bind(2, "Smith");
            query.bind(3, "Bryan");
            query.bind("@lname", "Jenkins");

            rs = query.execute();

            Assert::That(rs.size() > 0, IsTrue());

            query.reset();

            query.where("first_name = ? or last_name = $1 or last_name = @lname");
            query.bind(1, "Bob");
            query.bind("@lname", "Smith");

            rs = query.execute();

            Assert::That(rs.size() > 0, IsTrue());

        });
#endif
    });
}
SPEC_END;

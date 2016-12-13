#include <bandit/bandit.h>
#include "db.test.h"
#include "row.h"
#include "util.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

specification(joins, []() {
    describe("a join", []() {
        before_each([&]() {
            test::setup_current_session();

            test::user u1;

            u1.set("first_name", "Mike");
            u1.set("last_name", "Jones");

            u1.save();

            test::user u2;

            u2.set("first_name", "Jason");
            u2.set("last_name", "Hendrick");

            u2.save();

            insert_query query(test::current_session, "user_settings", {"user_id", "valid", "created_at"});

            time_t curr_time_val = time(0);

            sql_time curr_time(curr_time_val, sql_time::DATETIME);

            query.values(u1.id(), 1, curr_time);

            query.execute();
        });

        after_each([]() { test::teardown_current_session(); });

        it("can be copied", []() {
            join_clause clause("tablename");

            clause.on("columnA = columnB");

            join_clause other(clause);

            Assert::That(other.to_sql(), Equals(clause.to_sql()));

            other = clause;

            Assert::That(other.to_sql(), Equals(clause.to_sql()));
        });

        it("can be moved", []() {
            join_clause clause("tablename");

            clause.on("columnA = columnB");

            string test = clause.to_sql();

            join_clause other(std::move(clause));

            Assert::That(other.to_sql(), Equals(test));

            join_clause temp("tablename");

            temp.on("columnA = columnB");

            test = temp.to_sql();

            other = std::move(temp);

            Assert::That(other.to_sql(), Equals(test));
        });

        it("can join", []() {

            select_query query(test::current_session, {"u.id", "s.created_at"});

            query.from(test::user::TABLE_NAME, "u").join("user_settings", "s").on("u.id = s.user_id") and
                ("s.valid = 1");

            auto rs = query.execute();

            Assert::That(rs.empty(), IsFalse());
        });

        it("can be a cross join", []() {
            select_query query(test::current_session);

            query.from(test::user::TABLE_NAME, "u").join("user_settings", "s", join::cross).on("u.id = s.user_id") and
                ("s.valid = 1");

            auto rs = query.execute();

            Assert::That(rs.size(), Equals(2));
        });

        it("can be full outer", []() {
            if (test::current_session->has_feature(session::FEATURE_FULL_OUTER_JOIN)) {
                select_query query(test::current_session);

                query.from(test::user::TABLE_NAME, "u")
                        .join("user_settings", "s", join::full)
                        .on("u.id = s.user_id") and
                    ("s.valid = 1");

                auto rs = query.execute();

                Assert::That(rs.size(), Equals(2));
            }
        });
        it("can be a right", []() {
            if (test::current_session->has_feature(session::FEATURE_RIGHT_JOIN)) {
                select_query query(test::current_session);

                query.from(test::user::TABLE_NAME, "u")
                        .join("user_settings", "s", join::right)
                        .on("u.id = s.user_id") and
                    ("s.valid = 1");

                auto rs = query.execute();

                Assert::That(rs.size(), Equals(1));
            }
        });

        it("can be a left", []() {
            select_query query(test::current_session);

            query.from(test::user::TABLE_NAME, "u").join("user_settings", "s", join::left).on("u.id = s.user_id") and
                ("s.valid = 1");

            auto rs = query.execute();

            Assert::That(rs.size(), Equals(2));
        });

        it("can be a inner", []() {
            select_query query(test::current_session);

            query.from(test::user::TABLE_NAME, "u").join("user_settings", "s", join::inner).on("u.id = s.user_id") and
                ("s.valid = 1");

            auto rs = query.execute();

            Assert::That(rs.size(), Equals(1));
        });

        it("can be a natural", []() {
            select_query query(test::current_session);

            query.from(test::user::TABLE_NAME, "u").join("user_settings", "s", join::natural);

            auto rs = query.execute();

            Assert::That(rs.size(), Equals(1));
        });

        it("can be a default", []() {
            select_query query(test::current_session);

            query.from(test::user::TABLE_NAME, "u").join("user_settings", "s", join::none).on("u.id = s.user_id") and
                ("s.valid = 1");

            auto rs = query.execute();

            Assert::That(rs.size(), Equals(1));
        });

        it("can set the table name", []() {
            join_clause join;

            join.table("user_settings", "s").on("u.id = s.user_id");

            Assert::That(join.table(), Equals("user_settings s"));
        });
        it("can set the type", []() {
            join_clause join;

            join.type(join::left).table("user_settings s").on("u.id = s.user_id");

            Assert::That(join.type(), Equals(join::left));
        });
        it("can use a where clause", []() {
            join_clause join;
            where_clause where("u.id = s.user_id");

            where and ("s.valid = 1");

            join.table("user_settings s").on(where);

            Assert::That(join.on().to_sql(), Equals("u.id = s.user_id AND s.valid = 1"));
        });

        it("can reset", []() {
            join_clause join("user_settings s");

            join.on("u.id = s.user_id");

            Assert::That(join.to_sql(), Equals(" JOIN user_settings s ON u.id = s.user_id"));

            join.reset();

            Assert::That(join.empty(), IsTrue());
        });
    });
});

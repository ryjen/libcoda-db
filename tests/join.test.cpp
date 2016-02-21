#include <bandit/bandit.h>
#include "row.h"
#include "db.test.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;


go_bandit([]() {

    describe("a join", []() {
        before_each([&]() {
            setup_testdb();

            user u1(testdb);

            u1.set("first_name", "Mike");
            u1.set("last_name", "Jones");

            u1.save();

            insert_query query(testdb, "user_settings", {"user_id", "valid", "created_at"});

            time_t curr_time_val = time(0);

            sql_time curr_time(curr_time_val, sql_time::DATETIME);

            query.bind_all(u1.id(), 1, curr_time);

            query.execute();
        });
        after_each([]() { teardown_testdb(); });


        it("can join", []() {

            select_query query(testdb, "users u", {"u.id", "s.created_at"});

            query.join("user_settings s").on("u.id = s.user_id") and ("s.valid = 1");

            auto rs = query.execute();

            Assert::That(rs.empty(), IsFalse());
        });

        it("can be a cross join", []() {
            select_query query(testdb, "users u");

            query.join("user_settings s", join::cross);

            auto rs = query.execute();

            Assert::That(rs.size() >= 1, IsTrue());
        });

        it("can be full outer", []() {
            select_query query(testdb, "users u");

            query.join("user_settings s", join::full_outer);

            auto rs = query.execute();

            Assert::That(rs.size() >= 1, IsTrue());
        });

        it("can be a right outer", []() {
            select_query query(testdb, "users u");

            query.join("user_settings s", join::right_outer);

            auto rs = query.execute();

            Assert::That(rs.size() >= 1, IsTrue());
        });

        it("can be a left outer", []() {
            select_query query(testdb, "users u");

            query.join("user_settings s", join::left_outer);

            auto rs = query.execute();

            Assert::That(rs.size() >= 1, IsTrue());
        });

        it("can be a inner", []() {
            select_query query(testdb, "users u");

            query.join("user_settings s", join::inner);

            auto rs = query.execute();

            Assert::That(rs.size() >= 1, IsTrue());
        });

        it("can be a natural", []() {
            select_query query(testdb, "users u");

            query.join("user_settings s", join::natural);

            auto rs = query.execute();

            Assert::That(rs.size() >= 1, IsTrue());
        });

        it("can be a default", []() {
            select_query query(testdb, "users u");

            query.join("user_settings s", join::none);

            auto rs = query.execute();

            Assert::That(rs.size() >= 1, IsTrue());
        });
    });

});

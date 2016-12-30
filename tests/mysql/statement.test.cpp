
#include <bandit/bandit.h>
#include "../db.test.h"
#include "mysql/session.h"
#include "mysql/statement.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

SPEC_BEGIN(mysql_statement)
{
    describe("mysql statement", []() {
        before_each([]() {
            test::setup_current_session();

            test::user user1;

            user1.set_id(1);
            user1.set("first_name", "Bryan");
            user1.set("last_name", "Jenkins");

            user1.save();

            test::user user2;

            user2.set_id(3);

            user2.set("first_name", "Bob");
            user2.set("last_name", "Smith");

            user2.set("dval", 3.1456);

            user2.save();
        });

        after_each([]() { test::teardown_current_session(); });

        it("is movable", []() {
            mysql::statement stmt(dynamic_pointer_cast<mysql::session>(test::current_session->impl()));

            mysql::statement other(std::move(stmt));

            other.prepare("select * from users");

            AssertThrows(database_exception, stmt.prepare("select * from users"));

            stmt = std::move(other);

            stmt.prepare("select * from users");

            AssertThrows(database_exception, other.prepare("select * from users"));
        });

        it("can handle an error", []() {
            auto db = sqldb::create_session("mysql://xxxxxx/yyyyyy");

            mysql::statement stmt(dynamic_pointer_cast<mysql::session>(db->impl()));

            AssertThrows(database_exception, stmt.prepare("update qwerqwer set asdfsdf='1'"));
        });
    });
}
SPEC_END;

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#undef VERSION

#if defined(HAVE_LIBMYSQLCLIENT) && defined(TEST_MYSQL)

#include <bandit/bandit.h>
#include "db.test.h"
#include "mysql/statement.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

go_bandit([]() {
    describe("mysql statement", []() {
        before_each([]() {
            setup_current_session();

            user user1;

            user1.set_id(1);
            user1.set("first_name", "Bryan");
            user1.set("last_name", "Jenkins");

            user1.save();

            user user2;

            user2.set_id(3);

            user2.set("first_name", "Bob");
            user2.set("last_name", "Smith");

            user2.set("dval", 3.1456);

            user2.save();
        });

        after_each([]() { teardown_current_session(); });

        it("is movable", []() {
            mysql::statement stmt(dynamic_pointer_cast<mysql::session>(current_session));

            mysql::statement other(std::move(stmt));

            other.prepare("select * from users");

            AssertThrows(database_exception, stmt.prepare("select * from users"));

            stmt = std::move(other);

            stmt.prepare("select * from users");

            AssertThrows(database_exception, other.prepare("select * from users"));
        });

        it("can handle an error", []() {
            auto db = dynamic_pointer_cast<mysql::session>(sqldb::create_session("mysql://xxxxxx/yyyyyy"));

            mysql::statement stmt(db);

            AssertThrows(database_exception, stmt.prepare("update qwerqwer set asdfsdf='1'"));
        });
    });

});

#endif

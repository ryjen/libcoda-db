#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#undef VERSION

#ifdef HAVE_LIBMYSQLCLIENT

#include <bandit/bandit.h>
#include "../db.test.h"
#include "mysql/session.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;


go_bandit([]() {

    describe("mysql database", []() {
        before_each([]() { setup_current_session(); });
        after_each([]() { teardown_current_session(); });


        it("can handle bad parameters", []() {
            auto db = sqldb::create_session("mysql://zzzzz:zzzzz@zzzz/zzzzz:0");

            AssertThrows(database_exception, db->open());

            db->close();
        });

        it("can disable caching", []() {
            auto other = dynamic_pointer_cast<mysql::session>(current_session->impl());

            other->flags(0);

            Assert::That(other->flags(), Equals(0));

            insert_query insert(current_session);

            insert.into("users").columns({"first_name", "last_name"}).values("harry", "potter");

            Assert::That(insert.execute(), IsTrue());

            auto rs = current_session->query("select * from users");

            Assert::That(rs.size(), Equals(1));

            select_query select(current_session);

            select.from("users");

            rs = select.execute();

            Assert::That(rs.size(), Equals(1));
        });
    });

});

#endif

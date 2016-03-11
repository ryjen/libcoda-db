#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#undef VERSION

#if defined(HAVE_LIBMYSQLCLIENT) && defined(TEST_MYSQL)

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
        });

        it("can disable caching", []() {
            auto thesession = sqldb::create_session(get_env_uri("MYSQL_URI", "mysql://localhost/test"));

            auto other = dynamic_pointer_cast<mysql::session>(thesession->impl());

            other->open();

            other->flags(0);

            Assert::That(other->flags(), Equals(0));

            insert_query insert(thesession);

            insert.into("users").columns({"first_name", "last_name"}).values("harry", "potter");

            Assert::That(insert.execute(), IsTrue());

            auto rs = thesession->query("select * from users");

            Assert::That(rs.size(), Equals(1));

            select_query select(thesession);

            select.from("users");

            rs = select.execute();

            Assert::That(rs.size(), Equals(1));
        });
    });

});

#endif

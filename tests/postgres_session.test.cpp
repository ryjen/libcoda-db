#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#undef VERSION

#if defined(HAVE_LIBPQ) && defined(TEST_POSTGRES)

#include <bandit/bandit.h>
#include "db.test.h"
#include "postgres/session.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;


go_bandit([]() {

    describe("postgres database", []() {
        before_each([]() { setup_current_session(); });
        after_each([]() { teardown_current_session(); });

        it("can handle bad parameters", []() {
            auto db = sqldb::create_session<postgres::session>("postgres://zzzzz:zzzzz@zzzz/zzzzz:0");

            AssertThrows(database_exception, db->open());
        });
    });

});

#endif

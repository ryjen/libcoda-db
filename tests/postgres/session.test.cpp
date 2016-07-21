#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#undef VERSION

#ifdef HAVE_LIBPQ

#include <bandit/bandit.h>
#include "../db.test.h"
#include "postgres/session.h"

using namespace bandit;

using namespace std;

using namespace rj::db;


go_bandit([]() {

    describe("postgres database", []() {
        before_each([]() { setup_current_session(); });
        after_each([]() { teardown_current_session(); });

        it("can handle bad parameters", []() {
            auto db = sqldb::create_session("postgres://zzzzz:zzzzz@zzzz/zzzzz:0");

            AssertThrows(database_exception, db->open());
        });
    });

});

#endif

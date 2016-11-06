
#include <bandit/bandit.h>
#include "../db.test.h"
#include "mysql/session.h"

using namespace bandit;

using namespace std;

using namespace rj::db;


go_bandit([]() {

    describe("mysql database", []() {
        before_each([]() { setup_current_session(); });
        after_each([]() { teardown_current_session(); });


        it("can handle bad parameters", []() {
            auto db = sqldb::create_session("mysql://zzzzz:zzzzz@zzzz/zzzzz:0");

            AssertThrows(database_exception, db->open());

            db->close();
        });
    });

});

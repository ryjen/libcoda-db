/*!
 * @copyright ryan jennings (ryan-jennings.net), 2013 under LGPL
 */
#ifndef HAVE_CONFIG_H
#include "config.h"
#endif
#include <bandit/bandit.h>
#include "db.test.h"
#include "record.h"
#include "select_query.h"
#include "sqldb.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

std::shared_ptr<rj::db::session> current_session;

void setup_current_session()
{
    auto session = dynamic_pointer_cast<test_session>(current_session->impl());

    if (session) {
        session->setup();
    }
}

void teardown_current_session()
{
    auto session = dynamic_pointer_cast<test_session>(current_session->impl());

    if (session) {
        session->teardown();
    }

    current_session->clear_schema("users");
    current_session->clear_schema("user_settings");
}

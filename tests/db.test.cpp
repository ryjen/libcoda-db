/*!
 * @copyright ryan jennings (coda.life), 2013 under LGPL
 */
#include "db.test.h"
#include "record.h"
#include "select_query.h"
#include "spec_lib.h"
#include "sqldb.h"
#include <bandit/bandit.h>

using namespace bandit;

using namespace std;

using namespace coda::db;

using namespace snowhouse;

namespace coda::db::test {
      std::shared_ptr<coda::db::session> current_session;

      namespace spec {
        void load() {
#include "spec_lib.h"
        }
      } // namespace spec

      void setup_current_session() {
        auto session =
            dynamic_pointer_cast<test::session>(current_session->impl());

        if (session) {
          session->setup();
        }
      }

      void teardown_current_session() {
        auto session =
            dynamic_pointer_cast<test::session>(current_session->impl());

        if (session) {
          session->teardown();
        }

        current_session->clear_schema("users");
        current_session->clear_schema("user_settings");
      }
} // namespace coda::db::test

/*!
 * @copyright ryan jennings (coda.life), 2013 under LGPL
 */
#include <string>

#include "../db.test.h"
#include "record.h"
#include "select_query.h"
#include "sqldb.h"
#include "sqlite/session.h"
#include <bandit/bandit.h>

using namespace bandit;

using namespace std;

using namespace coda::db;

using namespace snowhouse;

namespace coda::db::test {
      void register_current_session() {
        auto sqlite_factory = std::make_shared<factory>();
        register_session("file", sqlite_factory);
        register_session("sqlite", sqlite_factory);

        current_session = coda::db::create_session("file://testdb.db");
      }

      void unregister_current_session() {}

      class sqlite_session : public coda::db::sqlite::session,
                             public test::session {
        public:
        using sqlite::session::session;

        void setup() override {
          open();
          execute("create table if not exists users(id integer primary key "
                  "autoincrement, first_name "
                  "varchar(45), last_name varchar(45), dval "
                  "real, data "
                  "blob, tval timestamp)");
          execute("create table if not exists user_settings(id integer primary "
                  "key autoincrement, user_id "
                  "integer not null, valid int(1), "
                  "created_at "
                  "timestamp)");
        }

        void teardown() override {
          close();
          unlink(connection_info().path.c_str());
        }
      };

      std::shared_ptr<coda::db::session_impl>
      factory::create(const coda::db::uri &value) {
        return std::make_shared<sqlite_session>(value);
      }
} // namespace coda::db::test

go_bandit([]() {
  describe("sqlite database", []() {
    it("can_parse_uri", []() {
      auto file = create_session("file://test.db");
      AssertThat(file != nullptr, IsTrue());
    });
  });
});

/*!
 * @copyright ryan jennings (ryan-jennings.net), 2013 under LGPL
 */
#include <bandit/bandit.h>
#include "db.test.h"
#include "record.h"
#include "select_query.h"
#include "sqldb.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

namespace rj
{
    namespace db
    {
        namespace test
        {
            void register_current_session()
            {
                auto sqlite_factory = std::make_shared<factory>();
                sqldb::register_session("file", sqlite_factory);
                sqldb::register_session("sqlite", sqlite_factory);

                current_session = rj::db::sqldb::create_session("file://testdb.db");
            }

            void unregister_current_session()
            {
            }

            class sqlite_session : public rj::db::sqlite::session, public test::session
            {
               public:
                using sqlite::session::session;

                void setup()
                {
                    open();
                    execute(
                        "create table if not exists users(id integer primary key autoincrement, first_name varchar(45), last_name varchar(45), dval "
                        "real, data "
                        "blob, tval timestamp)");
                    execute(
                        "create table if not exists user_settings(id integer primary key autoincrement, user_id integer not null, valid int(1), "
                        "created_at "
                        "timestamp)");
                }

                void teardown()
                {
                    close();
                    unlink(connection_info().path.c_str());
                }
            };

            std::shared_ptr<rj::db::session_impl> factory::create(const rj::db::uri &value)
            {
                return std::make_shared<sqlite_session>(value);
            }
        }
    }
}


SPEC_BEGIN(sqlite)
{
    describe("sqlite database", []() {
        it("can_parse_uri", []() {
            auto file = sqldb::create_session("file://test.db");
            AssertThat(file.get() != NULL, IsTrue());
        });
    });
}
SPEC_END;

/*!
 * @copyright ryan jennings (ryan-jennings.net), 2013 under LGPL
 */
#include <string>

#include <bandit/bandit.h>
#include "../db.test.h"
#include "../util.h"
#include "postgres/session.h"
#include "record.h"
#include "select_query.h"
#include "sqldb.h"

using namespace bandit;

using namespace std;

using namespace coda::db;

using namespace snowhouse;

namespace coda
{
    namespace db
    {
        namespace test
        {
            void register_current_session()
            {
                auto pq_factory = std::make_shared<test::factory>();
                sqldb::register_session("postgres", pq_factory);
                sqldb::register_session("postgresql", pq_factory);

                auto uri_s = get_env_uri("POSTGRES_URI", "postgres://localhost/test");
                current_session = coda::db::sqldb::create_session(uri_s);
                cout << "connecting to " << uri_s << endl;
            }

            void unregister_current_session()
            {
            }

            class postgres_session : public coda::db::postgres::session, public test::session
            {
               public:
                using postgres::session::session;

                void setup()
                {
                    open();
                    execute(
                        "create table if not exists users(id serial primary key unique, first_name varchar(45), "
                        "last_name varchar(45), dval real, "
                        "data bytea, "
                        "tval "
                        "timestamp)");

                    execute(
                        "create table if not exists user_settings(id serial primary key unique, user_id integer not "
                        "null, valid smallint, created_at "
                        "timestamp)");
                }

                void teardown()
                {
                    execute("drop table users");
                    execute("drop table user_settings");
                    close();
                }
            };

            std::shared_ptr<coda::db::session_impl> factory::create(const coda::db::uri &value)
            {
                return std::make_shared<postgres_session>(value);
            }
        }
    }
}
go_bandit([]() {
    it("can_parse_uri", []() {
        auto postgres = sqldb::create_session("postgres://localhost:4000/test");
        AssertThat(postgres.get() != NULL, IsTrue());
        AssertThat(postgres->connection_info().host, Equals("localhost"));
        AssertThat(postgres->connection_info().port, Equals("4000"));
        AssertThat(postgres->connection_info().path, Equals("test"));
    });

    describe("postgres database", []() {
        before_each([]() { test::setup_current_session(); });
        after_each([]() { test::teardown_current_session(); });

        it("can handle bad parameters", []() {
            auto db = sqldb::create_session("postgres://zzzzz:zzzzz@zzzz/zzzzz:0");

            AssertThrows(database_exception, db->open());
        });

    });
});

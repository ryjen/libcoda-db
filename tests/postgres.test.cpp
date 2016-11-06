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
#include "util.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

void register_current_session()
{
    auto pq_factory = std::make_shared<test_factory>();
    sqldb::register_session("postgres", pq_factory);
    sqldb::register_session("postgresql", pq_factory);

    auto uri_s = get_env_uri("POSTGRES_URI", "postgres://localhost/test");
    current_session = rj::db::sqldb::create_session(uri_s);
    cout << "connecting to " << uri_s << endl;
}

void unregister_current_session()
{
}

class postgres_test_session : public rj::db::postgres::session, public test_session
{
   public:
    using postgres::session::session;

    void setup()
    {
        open();
        execute(
            "create table if not exists users(id serial primary key unique, first_name varchar(45), last_name varchar(45), dval real, data bytea, "
            "tval "
            "timestamp)");

        execute(
            "create table if not exists user_settings(id serial primary key unique, user_id integer not null, valid smallint, created_at timestamp)");
    }

    void teardown()
    {
        execute("drop table users");
        execute("drop table user_settings");
        close();
    }
};

std::shared_ptr<rj::db::session_impl> test_factory::create(const rj::db::uri &value)
{
    return std::make_shared<postgres_test_session>(value);
}

go_bandit([]() {
    describe("postgres database", []() {
        it("can_parse_uri", []() {
            auto postgres = sqldb::create_session("postgres://localhost:4000/test");
            AssertThat(postgres.get() != NULL, IsTrue());
            AssertThat(postgres->connection_info().host, Equals("localhost"));
            AssertThat(postgres->connection_info().port, Equals("4000"));
            AssertThat(postgres->connection_info().path, Equals("test"));
        });
    });
});
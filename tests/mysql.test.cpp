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
    auto mysql_factory = std::make_shared<test_factory>();
    sqldb::register_session("mysql", mysql_factory);

    auto uri_s = get_env_uri("MYSQL_URI", "mysql://localhost/test");
    current_session = rj::db::sqldb::create_session(uri_s);
    cout << "connecting to " << uri_s << endl;
}

void unregister_current_session()
{
    mysql_library_end();
}

class mysql_test_session : public rj::db::mysql::session, public test_session
{
   public:
    using mysql::session::session;

    void setup()
    {
        open();
        execute(
            "create table if not exists users(id integer primary key auto_increment, first_name varchar(45), last_name varchar(45), dval real, data "
            "blob, tval timestamp)");
        execute(
            "create table if not exists user_settings(id integer primary key auto_increment, user_id integer not null, valid int(1), created_at "
            "timestamp)");
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
    return std::make_shared<mysql_test_session>(value);
}


go_bandit([]() {
    describe("mysql database", []() {
        it("can_parse_uri", []() {

            auto mysql = sqldb::create_session("mysql://localhost:4000/test");
            AssertThat(mysql.get() != NULL, IsTrue());
            AssertThat(mysql->connection_info().host, Equals("localhost"));
            AssertThat(mysql->connection_info().port, Equals("4000"));
            AssertThat(mysql->connection_info().path, Equals("test"));
        });
    });
});
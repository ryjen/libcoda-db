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

void register_test_sessions()
{
#if defined(HAVE_LIBSQLITE3)
    auto sqlite_factory = std::make_shared<test_sqlite3_factory>();
    sqldb::register_session("file", sqlite_factory);
    sqldb::register_session("sqlite", sqlite_factory);
#endif
#if defined(HAVE_LIBMYSQLCLIENT)
    auto mysql_factory = std::make_shared<test_mysql_factory>();
    sqldb::register_session("mysql", mysql_factory);
#endif
#if defined(HAVE_LIBPQ)
    auto pq_factory = std::make_shared<test_postgres_factory>();
    sqldb::register_session("postgres", pq_factory);
    sqldb::register_session("postgresql", pq_factory);
#endif
}
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

#if defined(HAVE_LIBSQLITE3)
std::shared_ptr<rj::db::session_impl> test_sqlite3_factory::create(const rj::db::uri &value)
{
    return std::make_shared<test_sqlite3_session>(value);
}

void test_sqlite3_session::setup()
{
    open();
    execute(
        "create table if not exists users(id integer primary key autoincrement, first_name varchar(45), last_name varchar(45), dval real, data "
        "blob, tval timestamp)");
    execute(
        "create table if not exists user_settings(id integer primary key autoincrement, user_id integer not null, valid int(1), created_at "
        "timestamp)");
}

void test_sqlite3_session::teardown()
{
    close();
    unlink(connection_info().path.c_str());
}
#endif

#if defined(HAVE_LIBMYSQLCLIENT)
std::shared_ptr<rj::db::session_impl> test_mysql_factory::create(const rj::db::uri &value)
{
    return std::make_shared<test_mysql_session>(value);
}

void test_mysql_session::setup()
{
    open();
    execute(
        "create table if not exists users(id integer primary key auto_increment, first_name varchar(45), last_name varchar(45), dval real, data "
        "blob, tval timestamp)");
    execute(
        "create table if not exists user_settings(id integer primary key auto_increment, user_id integer not null, valid int(1), created_at "
        "timestamp)");
}

void test_mysql_session::teardown()
{
    execute("drop table users");
    execute("drop table user_settings");
    close();
}
#endif

#if defined(HAVE_LIBPQ)
std::shared_ptr<rj::db::session_impl> test_postgres_factory::create(const rj::db::uri &value)
{
    return std::make_shared<test_postgres_session>(value);
}
void test_postgres_session::setup()
{
    open();
    execute(
        "create table if not exists users(id serial primary key unique, first_name varchar(45), last_name varchar(45), dval real, data bytea, tval "
        "timestamp)");

    execute("create table if not exists user_settings(id serial primary key unique, user_id integer not null, valid smallint, created_at timestamp)");
}

void test_postgres_session::teardown()
{
    execute("drop table users");
    execute("drop table user_settings");
    close();
}
#endif

go_bandit([]() {
    describe("database", []() {
        it("can_parse_uri", []() {
            try {
#ifdef HAVE_LIBSQLITE3
                auto file = sqldb::create_session("file://test.db");
                AssertThat(file.get() != NULL, IsTrue());
#endif
#ifdef HAVE_LIBMYSQLCLIENT
                auto mysql = sqldb::create_session("mysql://localhost:4000/test");
                AssertThat(mysql.get() != NULL, IsTrue());
                AssertThat(mysql->connection_info().host, Equals("localhost"));
                AssertThat(mysql->connection_info().port, Equals("4000"));
                AssertThat(mysql->connection_info().path, Equals("test"));
#endif
#ifdef HAVE_LIBPQ
                auto postgres = sqldb::create_session("postgres://localhost:4000/test");
                AssertThat(postgres.get() != NULL, IsTrue());
                AssertThat(postgres->connection_info().host, Equals("localhost"));
                AssertThat(postgres->connection_info().port, Equals("4000"));
                AssertThat(postgres->connection_info().path, Equals("test"));
#endif

            } catch (const std::exception &e) {
                cerr << e.what() << endl;
                throw e;
            }
        });
    });
});

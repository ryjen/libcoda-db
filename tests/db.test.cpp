/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef HAVE_CONFIG_H
#include "config.h"
#endif
#include <bandit/bandit.h>
#include "record.h"
#include "select_query.h"
#include "sqldb.h"
#include "db.test.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

#if defined(HAVE_LIBMYSQLCLIENT) && defined(TEST_MYSQL)
test_mysql_db mysql_testdb;
sqldb *testdb = &mysql_testdb;
#elif defined(HAVE_LIBSQLITE3) && defined(TEST_SQLITE)
test_sqlite3_db sqlite_testdb;
sqldb *testdb = &sqlite_testdb;
#elif defined(HAVE_LIBPQ) && defined(TEST_POSTGRES)
test_postgres_db postgres_testdb;
sqldb *testdb = &postgres_testdb;
#else
sqldb *testdb = nullptr;
#endif

std::random_device r;
std::default_random_engine rand_eng(r());

std::string get_env_uri(const char *name, const std::string &def)
{
    char *temp = getenv(name);

    if (temp != NULL) {
        return temp;
    }

    return def;
}

void setup_testdb()
{
    try {
        test_db *thisdb = dynamic_cast<test_db *>(testdb);

        thisdb->setup();
    } catch (const std::exception &e) {
        std::cout << e.what() << ": " << testdb->last_error() << std::endl;
        throw e;
    }
}

void teardown_testdb()
{
    try {
        test_db *thisdb = dynamic_cast<test_db *>(testdb);

        thisdb->teardown();
    } catch (const std::exception &e) {
        std::cout << e.what() << ": " << testdb->last_error() << std::endl;
        throw e;
    }
}

string random_name()
{
    char alpha[27] = "abcdefghijklmnopqrstuvwxyz";

    const int len = random_num(5, 9);
    char buf[20] = {0};

    for (int i = 0; i < len; i++) {
        int c = random_num<int>(0, 27);
        buf[i] = alpha[c];
    }
    buf[len] = 0;
    return string(buf) + std::to_string(random_num<int>(1000, 9999));
}

#if defined(HAVE_LIBSQLITE3) && defined(TEST_SQLITE)
void test_sqlite3_db::setup()
{
    open();
    execute(
        "create table if not exists users(id integer primary key autoincrement, first_name varchar(45), last_name varchar(45), dval real, data "
        "blob, tval timestamp)");
    execute(
        "create table if not exists user_settings(id integer primary key autoincrement, user_id integer not null, valid int(1), created_at "
        "timestamp)");
}

void test_sqlite3_db::teardown()
{
    close();
    unlink(connection_info().path.c_str());
    schemas()->clear("users");
    schemas()->clear("user_settings");
}
#endif

#if defined(HAVE_LIBMYSQLCLIENT) && defined(TEST_MYSQL)
void test_mysql_db::setup()
{
    open();
    execute(
        "create table if not exists users(id integer primary key auto_increment, first_name varchar(45), last_name varchar(45), dval real, data "
        "blob, tval timestamp)");
    execute(
        "create table if not exists user_settings(id integer primary key auto_increment, user_id integer not null, valid int(1), created_at "
        "timestamp)");
}

void test_mysql_db::teardown()
{
    execute("drop table users");
    execute("drop table user_settings");
    close();
    schemas()->clear("users");
    schemas()->clear("user_settings");
}
#endif

#if defined(HAVE_LIBPQ) && defined(TEST_POSTGRES)
void test_postgres_db::setup()
{
    open();
    execute(
        "create table if not exists users(id serial primary key unique, first_name varchar(45), last_name varchar(45), dval real, data bytea, tval "
        "timestamp)");

    execute("create table if not exists user_settings(id serial primary key unique, user_id integer not null, valid smallint, created_at timestamp)");
}

void test_postgres_db::teardown()
{
    execute("drop table users");
    execute("drop table user_settings");
    close();
    schemas()->clear("users");
    schemas()->clear("user_settings");
}
#endif

go_bandit([]() {
    describe("database", []() {
        before_each([]() { setup_testdb(); });

        after_each([]() { teardown_testdb(); });

        it("can_parse_uri", []() {
            try {
#ifdef HAVE_LIBSQLITE3
                auto file = sqldb::from_uri("file://test.db");
                AssertThat(file.get() != NULL, IsTrue());
#endif
#ifdef HAVE_LIBMYSQLCLIENT
                auto mysql = sqldb::from_uri("mysql://localhost:4000/test");
                AssertThat(mysql.get() != NULL, IsTrue());
                AssertThat(mysql->connection_info().host, Equals("localhost"));
                AssertThat(mysql->connection_info().port, Equals("4000"));
                AssertThat(mysql->connection_info().path, Equals("test"));
#endif
#ifdef HAVE_LIBPQ
                auto postgres = sqldb::from_uri("postgres://localhost:4000/test");
                AssertThat(postgres.get() != NULL, IsTrue());
                AssertThat(mysql->connection_info().host, Equals("localhost"));
                AssertThat(mysql->connection_info().port, Equals("4000"));
                AssertThat(mysql->connection_info().path, Equals("test"));
#endif

            } catch (const std::exception &e) {
                cerr << e.what() << endl;
                throw e;
            }
        });
    });
});

/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef HAVE_CONFIG_H
#include "config.h"
#endif
#include <bandit/bandit.h>
#include "base_record.h"
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

void setup_testdb()
{
    try {
        test_db *thisdb = dynamic_cast<test_db *>(testdb);

        thisdb->setup();
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        throw e;
    }
}

void teardown_testdb()
{
    try {
        test_db *thisdb = dynamic_cast<test_db *>(testdb);

        thisdb->teardown();
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        throw e;
    }
}

#if defined(HAVE_LIBSQLITE3) && defined(TEST_SQLITE)
void test_sqlite3_db::setup()
{
    open();
    execute(
        "create table if not exists users(id integer primary key autoincrement, first_name varchar(45), last_name varchar(45), dval real, data "
        "blob)");
}

void test_sqlite3_db::teardown()
{
    close();
    unlink(connection_info().path.c_str());
    schemas()->clear("users");
}
#endif

#if defined(HAVE_LIBMYSQLCLIENT) && defined(TEST_MYSQL)
void test_mysql_db::setup()
{
    open();
    execute(
        "create table if not exists users(id integer primary key auto_increment, first_name varchar(45), last_name varchar(45), dval real, data "
        "blob)");
}

void test_mysql_db::teardown()
{
    execute("drop table users");
    close();
    schemas()->clear("users");
}
#endif

#if defined(HAVE_LIBPQ) && defined(TEST_POSTGRES)
void test_postgres_db::setup()
{
    open();
    execute("create table if not exists users(id serial primary key unique, first_name varchar(45), last_name varchar(45), dval real, data bytea)");
}

void test_postgres_db::teardown()
{
    execute("drop table users");
    close();
    schemas()->clear("users");
}
#endif

go_bandit([]() {
    describe("database", []() {
        before_each([]() { setup_testdb(); });

        after_each([]() { teardown_testdb(); });
        it("can_parse_uri", []() {
            try {
                auto file = sqldb::from_uri("file://test.db");

                AssertThat(file.get() != NULL, IsTrue());

                auto mysql = sqldb::from_uri("mysql://localhost:4000/test");
                AssertThat(mysql.get() != NULL, IsTrue());

                auto postgres = sqldb::from_uri("postgres://localhost:4000/test");
                AssertThat(postgres.get() != NULL, IsTrue());

            } catch (const std::exception &e) {
                cerr << e.what() << endl;
                throw e;
            }
        });
    });
});

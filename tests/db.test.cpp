/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include <bandit/bandit.h>
#include "base_record.h"
#include "select_query.h"
#include "sqldb.h"
#include "db.test.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

sqldb *testdb = NULL;

#if defined(HAVE_LIBMYSQLCLIENT)
test_mysql_db mysql_testdb;
#endif

#if defined(HAVE_LIBSQLITE3)
test_sqlite3_db sqlite_testdb;
#endif

void setup_testdb()
{
    try {
#if defined(HAVE_LIBSQLITE3)
        sqlite_testdb.setup();
#endif
#if defined(HAVE_LIBMYSQLCLIENT)
        mysql_testdb.setup();
#endif
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        throw e;
    }
}

void teardown_testdb()
{
#if defined(HAVE_LIBSQLITE3)
    sqlite_testdb.teardown();
#endif
#if defined(HAVE_LIBMYSQLCLIENT)
    mysql_testdb.teardown();
#endif
}

#if defined(HAVE_LIBSQLITE3)
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
    unlink(connection_string().c_str());
    schemas()->clear("users");
}
#endif

#if defined(HAVE_LIBMYSQLCLIENT)
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

go_bandit([]() {
    describe("database", []() {
        before_each([]() { setup_testdb(); });

        after_each([]() { teardown_testdb(); });
        it("can_parse_uri", []() {
            try {
#ifdef HAVE_LIBSQLITE3
                auto file = get_db_from_uri("file://test.db");

                AssertThat(file.get() != NULL, IsTrue());
#endif
#ifdef HAVE_LIBMYSQLCLIENT
                auto mysql = get_db_from_uri("mysql://localhost:4000/test");
                AssertThat(mysql.get() != NULL, IsTrue());
#endif
            } catch (const std::exception &e) {
                cerr << e.what() << endl;
                throw e;
            }
        });
    });
});

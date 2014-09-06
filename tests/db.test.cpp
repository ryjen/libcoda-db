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

#if defined(TEST_MYSQL) && defined(HAVE_LIBMYSQLCLIENT)

test_mysql_db testdb2;
sqldb *testdb = &testdb2;
#else
test_sqlite3_db testdb1;
sqldb *testdb = &testdb1;
#endif

void setup_testdb()
{
    try
    {
#ifdef TEST_SQLITE
        testdb1.setup();
#endif
#if defined(TEST_MYSQL) && defined(HAVE_LIBMYSQLCLIENT)
        testdb2.setup();
#endif
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
        throw e;
    }
}

void teardown_testdb()
{
#ifdef TEST_SQLITE
    testdb1.teardown();
#endif
#if defined(TEST_MYSQL) && defined(HAVE_LIBMYSQLCLIENT)
    testdb2.teardown();
#endif
}

void test_sqlite3_db::setup()
{
    open();
    execute("create table if not exists users(id integer primary key autoincrement, first_name varchar(45), last_name varchar(45), dval real, data blob)");
}

void test_sqlite3_db::teardown()
{
    close();
    unlink(connection_string().c_str());
    schemas()->clear("users");
}

#ifdef HAVE_LIBMYSQLCLIENT
void test_mysql_db::setup()
{
    open();
    execute("create table if not exists users(id integer primary key auto_increment, first_name varchar(45), last_name varchar(45), dval real, data blob)");
}

void test_mysql_db::teardown()
{
    execute("drop table users");
    close();
    schemas()->clear("users");
}
#endif

go_bandit([]()
{
    describe("database", []()
    {
        before_each([]()
        {
            setup_testdb();
        });

        after_each([]()
        {
            teardown_testdb();
        });
        it("can_parse_uri", []()
        {
            try
            {
                auto db = get_db_from_uri("file://test.db");

                AssertThat(db.get() != NULL, IsTrue());
            }
            catch (const std::exception &e)
            {
                cerr << e.what() << endl;
                throw e;
            }
        });
    });
});


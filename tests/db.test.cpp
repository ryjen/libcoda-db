/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include <igloo/igloo.h>
#include "base_record.h"
#include "select_query.h"
#include "sqldb.h"
#include "db.test.h"

using namespace igloo;

using namespace std;

using namespace arg3::db;

test_sqlite3_db testdb1;
test_mysql_db testdb2;

#ifdef TEST_MYSQL
sqldb *testdb = &testdb2;
#else
sqldb *testdb = &testdb1;
#endif

void setup_testdb()
{
    try
    {
#ifdef TEST_SQLITE
        testdb1.setup();
#endif
#ifdef TEST_MYSQL
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
#ifdef TEST_MYSQL
    testdb2.teardown();
#endif
}

Context(sqldb_test)
{
    static void SetUpContext()
    {
        setup_testdb();

    }

    static void TearDownContext()
    {
        teardown_testdb();
    }

    Spec(can_parse_uri)
    {
        try
        {
            auto db = get_db_from_uri("file://test.db");

            Assert::That(dynamic_cast<sqlite3_db *>(db.get()) != NULL, Equals(true));
        }
        catch (const std::exception &e)
        {
            cerr << e.what() << endl;
            throw e;
        }

        // auto db2 = get_db_from_uri("mysql://user@localhost/dbname");

        //Assert::That(dynamic_cast<mysql_db *>(db2.get()) != NULL, Equals(true));
    }
};


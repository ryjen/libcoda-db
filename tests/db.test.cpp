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

};


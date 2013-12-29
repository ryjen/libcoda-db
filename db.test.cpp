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

testsqlite3db testdb1;
testmysqldb testdb2;

sqldb *testdb = &testdb1;

void setup_testdb()
{
    try
    {
        testdb1.setup();
        testdb2.setup();
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
        throw e;
    }
}

void teardown_testdb()
{
    testdb1.teardown();
    testdb2.teardown();
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


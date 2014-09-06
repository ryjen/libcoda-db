
#include <bandit/bandit.h>
#include "db.test.h"

using namespace bandit;

// go_bandit([]()
// {
//     before_each([]()
//     {
//         setup_testdb();
//     });

//     after_each([]()
//     {
//         teardown_testdb();
//     });
// });

int main(int argc, char *argv[])
{
#ifdef TEST_SQLITE
    testdb = &testdb1;
#elif defined(HAVE_LIBMYSQLCLIENT)
    testdb = &testdb2;
#else
    cout << "Mysql not supported" << endl;
    return 0;
#endif

#ifdef TEST_CACHE
    if(testdb) {
	testdb->set_cache_level(arg3::db::sqldb::CACHE_RESULTSET);
	cout << "setting cache level" << endl;
    }
#endif
    // return TestRunner::RunAllTests(argc, argv);
    return bandit::run(argc, argv);
}

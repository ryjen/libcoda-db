
#include <bandit/bandit.h>
#include "db.test.h"

using namespace bandit;

int main(int argc, char *argv[])
{
#ifdef TEST_SQLITE
#ifdef HAVE_LIBSQLITE3
    testdb = &sqlite_testdb;
#else
    cout << "Sqlite not supported" << endl;
    return 1;
#endif
#elif defined(TEST_MYSQL)
#ifdef HAVE_LIBMYSQLCLIENT
    testdb = &mysql_testdb;
#else
    cout << "Mysql not supported" << endl;
    return 1;
#endif
#endif
    
		if (!bandit::run(argc, argv)) {
			if (testdb) {
				testdb->set_cache_level(arg3::db::sqldb::CACHE_RESULTSET);
				cout << "setting cache level" << endl;
				return bandit::run(argc, argv);
			}
		  return 0;
		}
		
		return 1;
}

#include <bandit/bandit.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "db.test.h"

using namespace bandit;

#if !defined(HAVE_LIBMYSQLCLIENT) && !defined(HAVE_LIBSQLITE3)
#error "Mysql or sqlite is not installed on the system"
#endif

int run_db_test(int argc, char *argv[])
{
    if (!testdb) {
        return 1;
    }

    // run the uncached test
    if (bandit::run(argc, argv)) {
        return 1;
    }

    // run the cached test
    testdb->set_cache_level(arg3::db::sqldb::CACHE_RESULTSET);
    cout << "setting cache level" << endl;
    return bandit::run(argc, argv);
}

int main(int argc, char *argv[])
{
#ifdef HAVE_LIBSQLITE3
    puts("running sqlite3 tests");
    testdb = &sqlite_testdb;
    if (run_db_test(argc, argv)) {
        return 1;
    }
#else
    cout << "Sqlite not supported" << endl;
#endif

#ifdef HAVE_LIBMYSQLCLIENT
    puts("running mysql tests");
    testdb = &mysql_testdb;
    if (run_db_test(argc, argv)) {
        return 1;
    }
#else
    cout << "Mysql not supported" << endl;
#endif

    return 0;
}

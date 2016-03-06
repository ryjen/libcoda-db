#include <bandit/bandit.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "db.test.h"

using namespace bandit;
using namespace std;

#if !defined(HAVE_LIBMYSQLCLIENT) && !defined(HAVE_LIBSQLITE3) && !defined(HAVE_LIBPQ)
#error "Mysql, postgres or sqlite is not installed on the system"
#endif

int main(int argc, char *argv[])
{
    int opt = getopt(argc, argv, "l::");

    if (opt == 'l') {
        arg3::db::log::set_level(optarg);
    } else {
        arg3::db::log::set_level(arg3::db::log::Error);
    }
#ifdef HAVE_LIBSQLITE3
#ifdef TEST_SQLITE
    puts("running sqlite3 tests");
    testdb = &sqlite_testdb;
    // run the uncached test
    if (bandit::run(argc, argv)) {
        return 1;
    }

    // run the cached test
    sqlite_testdb.cache_level(arg3::db::sqlite::cache::ResultSet);
    cout << "setting cache level" << endl;
    if (bandit::run(argc, argv)) {
        return 1;
    }
#endif
#else
    cout << "Sqlite not supported" << endl;
#endif

#ifdef HAVE_LIBMYSQLCLIENT
#ifdef TEST_MYSQL
    puts("running mysql tests");
    char *host = std::getenv("MYSQL_URI");
    if (host) {
        mysql_testdb.set_connection_info(arg3::db::uri(host));
        cout << "connecting to " << mysql_testdb.connection_info().value << endl;
    }
    testdb = &mysql_testdb;
    if (bandit::run(argc, argv)) {
        return 1;
    }
#endif
#else
    cout << "Mysql not supported" << endl;
#endif

#ifdef HAVE_LIBPQ
#ifdef TEST_POSTGRES
    puts("running postgres tests");
    char *host = std::getenv("POSTGRES_URI");
    if (host) {
        postgres_testdb.set_connection_info(arg3::db::uri(host));
        cout << "connecting to " << postgres_testdb.connection_info().value << endl;
    }
    testdb = &postgres_testdb;
    if (bandit::run(argc, argv)) {
        return 1;
    }
#endif
#else
    cout << "Postgres not supported" << endl;
#endif
    return 0;
}

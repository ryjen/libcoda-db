#include <bandit/bandit.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "db.test.h"
#include "sqldb.h"
#include "sqlite/session.h"

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

    register_test_sessions();

#ifdef HAVE_LIBSQLITE3
#ifdef TEST_SQLITE
    puts("running sqlite3 tests");

    auto sqlite_session = arg3::db::sqldb::create_session<arg3::db::sqlite::session>("file://testdb.db");

    current_session = sqlite_session;

    // run the uncached test
    if (bandit::run(argc, argv)) {
        return 1;
    }

    // run the cached test
    sqlite_session->cache_level(arg3::db::sqlite::cache::ResultSet);

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

    auto uri_s = get_env_uri("MYSQL_URI", "mysql://localhost/test");
    current_session = arg3::db::sqldb::create_session(uri_s);
    cout << "connecting to " << uri_s << endl;

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

    auto uri_s = get_env_uri("POSTGRES_URI", "postgres://localhost/test");
    current_session = arg3::db::sqldb::create_session(uri_s);
    cout << "connecting to " << uri_s << endl;

    if (bandit::run(argc, argv)) {
        return 1;
    }
#endif
#else
    cout << "Postgres not supported" << endl;
#endif
    return 0;
}

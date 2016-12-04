
#include <bandit/bandit.h>
#include "db.test.h"
#include "log.h"
#include "sqldb.h"
#include "sqlite/session.h"

using namespace bandit;
using namespace std;

int main(int argc, char *argv[])
{
    int opt = getopt(argc, argv, "l::");

    if (opt == 'l') {
        rj::db::log::set_level(optarg);
    } else {
        rj::db::log::set_level(rj::db::log::Error);
    }

    rj::db::test::spec::load();

    rj::db::test::register_current_session();

    if (bandit::run(argc, argv)) {
        return EXIT_FAILURE;
    }

    rj::db::test::unregister_current_session();

    return EXIT_SUCCESS;
}

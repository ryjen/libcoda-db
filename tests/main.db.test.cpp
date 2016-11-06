#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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

    register_current_session();

    if (bandit::run(argc, argv)) {
        return EXIT_FAILURE;
    }

    unregister_current_session();

    return EXIT_SUCCESS;
}

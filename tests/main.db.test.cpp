#include <string>

#include <bandit/bandit.h>
#include "db.test.h"
#include "sqldb.h"

using namespace bandit;
using namespace std;

int main(int argc, char *argv[])
{
    int opt = getopt(argc, argv, "l::");

    if (opt == 'l') {
        coda::db::log::set_level(optarg);
    } else {
        coda::db::log::set_level(coda::db::log::Error);
    }

    coda::db::test::spec::load();

    coda::db::test::register_current_session();

    if (bandit::run(argc, argv)) {
        return EXIT_FAILURE;
    }

    coda::db::test::unregister_current_session();

    return EXIT_SUCCESS;
}

#include <string>

#include "db.test.h"
#include "sqldb.h"
#include <bandit/bandit.h>

using namespace bandit;
using namespace std;

int main(int argc, char *argv[]) {
  coda::db::test::spec::load();

  coda::db::test::register_current_session();

  int rval = EXIT_SUCCESS;

  if (bandit::run(argc, argv)) {
    rval = EXIT_FAILURE;
  }

  coda::db::test::unregister_current_session();

  return rval;
}

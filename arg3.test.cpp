
#include <igloo/igloo.h>
#include "db.test.h"

using namespace igloo;

int main(int argc, const char *argv[])
{
#ifdef TEST_SQLITE
    testdb = &testdb1;
#else
    testdb = &testdb2;
#endif
    return TestRunner::RunAllTests(argc, argv);
}
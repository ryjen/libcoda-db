#include <bandit/bandit.h>
#include "row.h"
#include "db.test.h"
#include "log.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

extern FILE *log::file;

bool test_last_log(log::level level, const char *test)
{
    char buf[BUFSIZ] = {0};
    char type[BUFSIZ] = {0};
    char logstr[BUFSIZ] = {0};

    fseek(log::file, 0, SEEK_SET);

    if (fgets(buf, BUFSIZ, log::file) == nullptr) {
        return false;
    }

    // setup for next log
    fseek(log::file, 0, SEEK_SET);

    sscanf(buf, "%*s %*s %[^:]: %[^\n]", type, logstr);

    if (strcmp(type, log::LevelNames[level])) {
        return false;
    }
    return !strcmp(logstr, test);
}

go_bandit([]() {

    before_each([]() { log::file = tmpfile(); });
    after_each([]() {
        fclose(log::file);
        log::file = stdout;
    });

    describe("a log", []() {
        it("can set level to trace", []() {
            log::set_level(log::Trace);

            log::trace("this is a trace");

            Assert::That(test_last_log(log::Trace, "this is a trace"), IsTrue());
        });

        it("can set level to debug", []() {

            log::set_level(log::Debug);

            log::debug("this is a debug");

            Assert::That(test_last_log(log::Debug, "this is a debug"), IsTrue());

            log::trace("this is a trace");

            Assert::That(test_last_log(log::Trace, "this is a trace"), IsFalse());
        });

        it("can set level to warn", []() {

            log::set_level(log::Warn);

            log::warn("this is a warn");

            Assert::That(test_last_log(log::Warn, "this is a warn"), IsTrue());

            log::trace("this is a trace");

            Assert::That(test_last_log(log::Trace, "this is a trace"), IsFalse());
        });

        it("can set level to error", []() {

            log::set_level(log::Error);

            log::error("this is an error");

            Assert::That(test_last_log(log::Error, "this is an error"), IsTrue());

            log::trace("this is a trace");

            Assert::That(test_last_log(log::Trace, "this is a trace"), IsFalse());
        });

        it("can set level to none", []() {

            log::set_level(log::None);

            log::error("this is an error");

            Assert::That(test_last_log(log::Error, "this is an error"), IsFalse());

        });

        it("can log an error number", []() {
            log::set_level(log::Error);

            log::error(ENOMEM);

            char buf[BUFSIZ] = {0};
            snprintf(buf, BUFSIZ, "%s (%d)", strerror(ENOMEM), ENOMEM);

            Assert::That(test_last_log(log::Error, buf), IsTrue());
        });
    });
});

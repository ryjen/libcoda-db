#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bandit/bandit.h>
#include <sqldb.h>
#include "log.h"
#include "row.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

#ifdef ENABLE_LOGGING
extern FILE *log::file;
#endif

bool test_last_log(log::level level, const char *test)
{
#ifdef ENABLE_LOGGING

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
#else
    return true;
#endif
}

go_bandit([]() {

#ifdef ENABLE_LOGGING
    before_each([]() { log::file = tmpfile(); });
    after_each([]() {

        fclose(log::file);
        log::file = stdout;
    });
#endif

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

#ifdef ENABLE_LOGGING
            log::trace("this is a trace");

            Assert::That(test_last_log(log::Trace, "this is a trace"), IsFalse());
#endif
        });

        it("can set level to warn", []() {

            log::set_level(log::Warn);

            log::warn("this is a warn");

            Assert::That(test_last_log(log::Warn, "this is a warn"), IsTrue());

            log::trace("this is a trace");

#ifdef ENABLE_LOGGING
            Assert::That(test_last_log(log::Trace, "this is a trace"), IsFalse());
#endif
        });

        it("can set level to error", []() {

            log::set_level(log::Error);

            log::error("this is an error");

            Assert::That(test_last_log(log::Error, "this is an error"), IsTrue());

            log::trace("this is a trace");

#ifdef ENABLE_LOGGING
            Assert::That(test_last_log(log::Trace, "this is a trace"), IsFalse());
#endif
        });

        it("can set level to none", []() {

            log::set_level(log::None);

            log::error("this is an error");

#ifdef ENABLE_LOGGING
            Assert::That(test_last_log(log::Error, "this is an error"), IsFalse());
#endif

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

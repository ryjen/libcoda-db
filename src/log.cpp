#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/time.h>
#include <time.h>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include "log.h"
#include "sqldb.h"

namespace rj
{
    namespace db
    {
        namespace log
        {
#ifdef ENABLE_LOGGING
            const char *LevelNames[] = {"UNKNOWN", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"};

            FILE *file = stdout;

            namespace current
            {
                level level;
            }
#endif
            void set_level(log::level level)
            {
#ifdef ENABLE_LOGGING
                current::level = level;
#endif
            }

            void set_level(const char *arg)
            {
#ifdef ENABLE_LOGGING
                if (arg == NULL || *arg == 0) {
                    return;
                }

                if (!strcasecmp(arg, "ERROR")) {
                    current::level = Error;
                } else if (!strcasecmp(arg, "WARN")) {
                    current::level = Warn;
                } else if (!strcasecmp(arg, "INFO")) {
                    current::level = Info;
                } else if (!strcasecmp(arg, "DEBUG")) {
                    current::level = Debug;
                } else if (!strcasecmp(arg, "TRACE")) {
                    current::level = Trace;
                }
#endif
            }

#ifdef ENABLE_LOGGING
            static void lvargs(log::level level, const char *const format, va_list args)
            {
                char buf[80] = {0};
                timeval curTime;

                if (!format || !*format || !log::file) {
                    return;
                }

                gettimeofday(&curTime, NULL);

                strftime(buf, 80, "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));

                fprintf(log::file, "%s:%03d %s: ", buf, unsigned(curTime.tv_usec / 1000), LevelNames[level]);

                vfprintf(log::file, format, args);
                fputs("\n", log::file);
                fflush(log::file);
            }
#endif
            void error(const char *const format, ...)
            {
#ifdef ENABLE_LOGGING
                va_list args;

                if (Error > current::level || !format || !*format) {
                    return;
                }

                va_start(args, format);
                lvargs(Error, format, args);
                va_end(args);
#endif
            }

            void warn(const char *const format, ...)
            {
#ifdef ENABLE_LOGGING
                va_list args;

                if (Warn > current::level || !format || !*format) {
                    return;
                }

                va_start(args, format);
                lvargs(Warn, format, args);
                va_end(args);
#endif
            }

            void info(const char *const format, ...)
            {
#ifdef ENABLE_LOGGING
                va_list args;

                if (Info > current::level || !format || !*format) {
                    return;
                }

                va_start(args, format);
                lvargs(Info, format, args);
                va_end(args);
#endif
            }

            void debug(const char *const format, ...)
            {
#ifdef ENABLE_LOGGING
                va_list args;

                if (Debug > current::level || !format || !*format) {
                    return;
                }

                va_start(args, format);
                lvargs(Debug, format, args);
                va_end(args);
#endif
            }

            void trace(const char *const format, ...)
            {
#ifdef ENABLE_LOGGING
                va_list args;

                if (Trace > current::level || !format || !*format) {
                    return;
                }

                va_start(args, format);
                lvargs(Trace, format, args);
                va_end(args);
#endif
            }

            void error(int errnum)
            {
#ifdef ENABLE_LOGGING
                error("%s (%d)", strerror(errnum), errnum);
#endif
            }
        }
    }
}

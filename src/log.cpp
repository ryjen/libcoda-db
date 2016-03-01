#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <time.h>
#include "sqldb.h"
#include "log.h"

namespace arg3
{
    namespace db
    {
        namespace log
        {
            const char *LevelNames[] = {"UNKNOWN", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"};

            FILE *file = stdout;

            namespace current
            {
                level level;
            }

            void set_level(log::level level)
            {
                current::level = level;
            }

            void set_level(const char *arg)
            {
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
            }

            static void lvargs(log::level level, const char *const format, va_list args)
            {
                char buf[BUFSIZ + 1] = {0};

                time_t t = 0;

                if (!format || !*format || !log::file) {
                    return;
                }

                t = time(0);

                strftime(buf, BUFSIZ, "%Y-%m-%d %H:%M:%S", localtime(&t));

                fprintf(log::file, "%s %s: ", buf, LevelNames[level]);

                vfprintf(log::file, format, args);
                fputs("\n", log::file);
                fflush(log::file);
            }

            void error(const char *const format, ...)
            {
                va_list args;

                if (Error > current::level || !format || !*format) {
                    return;
                }

                va_start(args, format);
                lvargs(Error, format, args);
                va_end(args);
            }

            void warn(const char *const format, ...)
            {
                va_list args;

                if (Warn > current::level || !format || !*format) {
                    return;
                }

                va_start(args, format);
                lvargs(Warn, format, args);
                va_end(args);
            }

            void info(const char *const format, ...)
            {
                va_list args;

                if (Info > current::level || !format || !*format) {
                    return;
                }

                va_start(args, format);
                lvargs(Info, format, args);
                va_end(args);
            }

            void debug(const char *const format, ...)
            {
                va_list args;

                if (Debug > current::level || !format || !*format) {
                    return;
                }

                va_start(args, format);
                lvargs(Debug, format, args);
                va_end(args);
            }

            void trace(const char *const format, ...)
            {
                va_list args;

                if (Trace > current::level || !format || !*format) {
                    return;
                }

                va_start(args, format);
                lvargs(Trace, format, args);
                va_end(args);
            }

            void error(int errnum)
            {
                error("%s (%d)", strerror(errnum), errnum);
            }
        }
    }
}

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

            namespace current
            {
                Level level;
            }

            void set_level(Level level)
            {
                current::level = level;
            }

            static void lvargs(Level level, const char *const format, va_list args)
            {
                char buf[BUFSIZ + 1] = {0};

                time_t t = 0;

                if (!format || !*format) {
                    return;
                }

                t = time(0);

                strftime(buf, BUFSIZ, "%Y-%m-%d %H:%M:%S", localtime(&t));

                fprintf(stdout, "%s %s: ", buf, LevelNames[level]);

                vfprintf(stdout, format, args);
                fputs("\n", stdout);
                fflush(stdout);
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

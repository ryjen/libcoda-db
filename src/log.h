#ifndef ARG3_DB_LOG_H_
#define ARG3_DB_LOG_H_

#include <cstring>

#ifndef __attribute__
#define __attribute__(x)
#endif

namespace arg3
{
    namespace db
    {

        namespace log
        {
            void error(const char *const format, ...) __attribute__((format(printf, 1, 2)));

            void warn(const char *const format, ...) __attribute__((format(printf, 1, 2)));

            void info(const char *const format, ...) __attribute__((format(printf, 1, 2)));

            void debug(const char *const format, ...) __attribute__((format(printf, 1, 2)));

            void trace(const char *const format, ...) __attribute__((format(printf, 1, 2)));

            void error(int errnum);
        }
    }
}

#endif
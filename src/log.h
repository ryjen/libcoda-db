/*!
 * @file log.h
 * internal logging for the library
 */
#ifndef RJ_DB_LOG_H
#define RJ_DB_LOG_H

#include <cstdio>
#include <cstring>

namespace rj
{
    namespace db
    {
        namespace log
        {
            extern FILE *file;

            extern const char *LevelNames[];

            /* standard printf style functions */

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

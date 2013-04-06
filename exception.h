/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_EXCEPTION_H_
#define _ARG3_DB_EXCEPTION_H_

#include <exception>
#include <string>

using namespace std;

namespace arg3
{
    namespace db
    {
        class database_exception : public std::exception
        {

        };

        class illegal_state_exception : public std::exception
        {
        private:
            string what_;
        public:
            illegal_state_exception(const string &what) : what_(what) {}

            const char *what() const throw()
            {
                return what_.c_str();
            }
        };

        class no_such_column_exception : public std::exception
        {

        };

        class record_not_found_exception : public std::exception
        {

        };
    }
}

#endif

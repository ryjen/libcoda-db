/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_EXCEPTION_H_
#define _ARG3_DB_EXCEPTION_H_

#define ARG3_DECLARE_EXCEPTION(CLASS, BASE) \
class CLASS: public BASE \
{ \
private: std::string what_; std::string context_; \
public: \
    CLASS(const std::string &what); \
    CLASS(const std::string &what, const std::string &context); \
    CLASS(); \
}

#define ARG3_IMPLEMENT_EXCEPTION(CLASS, BASE) \
CLASS::CLASS(const std::string &what) : BASE(), what_(what) {} \
CLASS::CLASS(const std::string &what, const std::string &context) : BASE(), what_(what), context_(context) {} \
CLASS::CLASS() {}
#include <string>

using namespace std;

namespace arg3
{
    namespace db
    {
        ARG3_DECLARE_EXCEPTION(database_exception, std::exception);

        ARG3_DECLARE_EXCEPTION(no_such_column_exception, database_exception);

        ARG3_DECLARE_EXCEPTION(record_not_found_exception, database_exception);

        ARG3_DECLARE_EXCEPTION(binding_error, database_exception);
    }
}

#endif

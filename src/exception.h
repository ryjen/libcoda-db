/*!
 * @file exception.h
 * declaration of exceptions used in the library
 * @copyright ryan jennings (ryan-jennings.net), 2013
 */
#ifndef CODA_DB_EXCEPTION_H
#define CODA_DB_EXCEPTION_H

#include <string>

#define CODA_DECLARE_EXCEPTION(CLASS, BASE)                           \
    class CLASS : public BASE                                       \
    {                                                               \
       private:                                                     \
        std::string what_;                                          \
        std::string context_;                                       \
                                                                    \
       public:                                                      \
        CLASS(const std::string &what);                             \
        CLASS(const std::string &what, const std::string &context); \
        CLASS();                                                    \
        virtual const char *what() const throw();                  \
        virtual const char *context() const throw();               \
    }

#define CODA_IMPLEMENT_EXCEPTION(CLASS, BASE)                                                                    \
    CLASS::CLASS(const std::string &what) : BASE(), what_(what)                                                \
    {                                                                                                          \
    }                                                                                                          \
    CLASS::CLASS(const std::string &what, const std::string &context) : BASE(), what_(what), context_(context) \
    {                                                                                                          \
    }                                                                                                          \
    CLASS::CLASS()                                                                                             \
    {                                                                                                          \
    }                                                                                                          \
    const char *CLASS::what() const noexcept                                                                   \
    {                                                                                                          \
        return what_.c_str();                                                                                  \
    }                                                                                                          \
    const char *CLASS::context() const noexcept                                                                \
    {                                                                                                          \
        return context_.c_str();                                                                               \
    }


namespace coda {
    namespace db {
        CODA_DECLARE_EXCEPTION(database_exception, std::exception);

        CODA_DECLARE_EXCEPTION(no_such_column_exception, database_exception);

        CODA_DECLARE_EXCEPTION(record_not_found_exception, database_exception);

        CODA_DECLARE_EXCEPTION(binding_error, database_exception);

        CODA_DECLARE_EXCEPTION(transaction_exception, database_exception);

        CODA_DECLARE_EXCEPTION(no_primary_key_exception, database_exception);

        CODA_DECLARE_EXCEPTION(value_conversion_error, database_exception);
    }
}

#endif

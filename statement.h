#ifndef ARG3_DB_STATEMENT_H_
#define ARG3_DB_STATEMENT_H_

#include "bindable.h"
#include "resultset.h"

namespace arg3
{
    namespace db
    {
        /*!
         * base class for a database specific implementation of a prepared statement
         */
        class statement : public bindable
        {
        public:
            statement() = default;
            statement(const statement &other) = default;
            statement(statement &&other) = default;
            virtual ~statement() = default;
            statement &operator=(const statement &other) = default;
            statement &operator=(statement && other) = default;
            virtual void prepare(const std::string &sql) = 0;
            virtual void finish() = 0;
            virtual void reset() = 0;
            virtual bool is_valid() const = 0;
            virtual resultset results() = 0;
            virtual bool result() = 0;
            virtual int last_number_of_changes() = 0;
            virtual string last_error() = 0;
        };
    }
}

#endif

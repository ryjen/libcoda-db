/*!
 * @file statement.h
 * an interface for a query statement
 */
#ifndef ARG3_DB_STATEMENT_H
#define ARG3_DB_STATEMENT_H

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
            typedef arg3::db::resultset resultset_type;

           public:
            /*!
             * prepares this statement for execution
             * @param sql the sql to prepare
             */
            virtual void prepare(const std::string &sql) = 0;

            /*!
             * releases resources for this statement
             */
            virtual void finish() = 0;

            /*!
             * resets this statement for a new execution
             */
            virtual void reset() = 0;

            /*!
             * tests if this statement is valid
             * @return true if the internals are valid
             */
            virtual bool is_valid() const = 0;

            /*!
             * executes this statement
             * @return a set of the results
             */
            virtual resultset_type results() = 0;

            /*!
             * executes this statement
             * @return true if successful
             */
            virtual bool result() = 0;

            /*!
             * gets the last number of changes
             * @return the last number of changes or zero
             */
            virtual int last_number_of_changes() = 0;

            /*!
             * gets the last error
             * @return the last error or an empty string
             */
            virtual std::string last_error() = 0;

            /*!
             * gets the last insert id
             * @return the last insert id or zero
             */
            virtual long long last_insert_id() = 0;
        };
    }
}

#endif

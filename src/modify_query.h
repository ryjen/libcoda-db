/*!
 * @file modify_query.h
 * representation of sql queries that modify data
 * @copyright ryan jennings (arg3.com), 2013
 */
#ifndef ARG3_DB_MODIFY_QUERY_H
#define ARG3_DB_MODIFY_QUERY_H

#include "query.h"
#include "sqldb.h"
#include "where_clause.h"

namespace arg3
{
    namespace db
    {
        /*!
         * a query to modify a table
         */
        class modify_query : public query
        {
           public:
            /*!
             * query flags
             */
            /*! perform subsequent executes in batches */
            constexpr static const int Batch = (1 << 0);

            /*!
             * @param db the database in use
             */
            modify_query(sqldb *db);

            /*!
             * @param schema the schema to modify
             */
            modify_query(const std::shared_ptr<schema> &schema);

            /* boilerplate */
            modify_query(const modify_query &other);
            modify_query(modify_query &&other);
            virtual ~modify_query();
            modify_query &operator=(const modify_query &other);
            modify_query &operator=(modify_query &&other);

            /*!
             * @return a string/sql representation of this query
             */
            virtual std::string to_string() const = 0;

            /*!
             * sets flags for this query (@see query flags)
             */
            modify_query &flags(int value);

            /*!
             * gets the flags for this query
             * @return the query flags
             */
            int flags() const;

            /*!
             * executes this query using a replace statement
             * @return the last number of changes made by this query
             */
            virtual int execute();

            /*!
             * @return the last number of changes made by this query
             */
            int last_number_of_changes() const;

           protected:
            int flags_;
            int numChanges_;
        };
    }
}

#endif

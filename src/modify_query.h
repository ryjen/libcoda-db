/*!
 * @file modify_query.h
 * representation of sql queries that modify data
 * @copyright ryan jennings (ryan-jennings.net), 2013
 */
#ifndef RJ_DB_MODIFY_QUERY_H
#define RJ_DB_MODIFY_QUERY_H

#include "query.h"

namespace rj
{
    namespace db
    {
        class session;

        /*!
         * a query to modify a table
         */
        class modify_query : public query
        {
           public:
            /*!
             * @param db the database in use
             */
            modify_query(const std::shared_ptr<rj::db::session> &session);

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

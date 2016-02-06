/*!
 * @file select_query.h
 * @copyright ryan jennings (arg3.com), 2013
 */
#ifndef ARG3_DB_SELECT_QUERY_H
#define ARG3_DB_SELECT_QUERY_H

#include "query.h"
#include "resultset.h"
#include "where_clause.h"
#include "join_clause.h"

namespace arg3
{
    namespace db
    {
        class sqldb;

        /*!
         * a query to select values from a table
         */
        class select_query : public query
        {
           public:
            friend class resultset;

           private:
            where_clause where_;
            join_clause join_;
            std::string limit_;
            std::string orderBy_;
            std::string groupBy_;
            std::string joinBy_;
            std::vector<std::string> columns_;
            std::string tableName_;

           public:
            /*!
             * @param db        the database in use
             * @param tableName the table name to query
             * @param columns   the columns to query
             */
            select_query(sqldb *db, const std::string &tableName, const std::vector<std::string> &columns);

            /*!
             * @param db        the database in use
             * @param tableName the table name to query
             */
            select_query(sqldb *db, const std::string &tableName);

            /*!
             * @param schema    the schema to query
             */
            select_query(const std::shared_ptr<schema> &schema);

            /* boilerplate */
            select_query(const select_query &other);
            select_query(select_query &&other);
            virtual ~select_query();
            select_query &operator=(const select_query &other);
            select_query &operator=(select_query &&other);

            /*!
             * @return the columns being queried
             */
            std::vector<std::string> columns() const;

            /*!
             * gets the limit clause for the query
             * @return the limit sql string
             */
            std::string limit() const;

            /*!
             * gets the order by clause for the query
             * @return the order by sql string
             */
            std::string order_by() const;

            /*!
             * gets the group by clause for the query
             * @return the group by sql string
             */
            std::string group_by() const;

            /*!
             * adds a where clause to this query
             * @param  value the sql string
             * @return       a reference to this
             */
            where_clause &where(const string &value);

            /*!
             * adds a where clause to this query
             * @param  value the second where clause
             * @return       a reference to this
             */
            select_query &where(const where_clause &value);

            /*!
             * sets the limit by clause for this query
             * @param  value the limit sql string
             * @return       a reference to this
             */
            select_query &limit(const std::string &value);

            /*!
             * sets the order by clause for this query
             * @param  value the order by sql string
             * @return       a reference to this
             */
            select_query &order_by(const std::string &value);

            /*!
             * sets the group by clause for this query
             * @param  value the group by sql string
             * @return       a reference to this
             */
            select_query &group_by(const std::string &value);

            /*!
             * sets the join clause for this query
             * @param  tableName the table name to join
             * @param  type      the type of join
             * @return           a join clause to perform additional modification
             */
            join_clause &join(const std::string &tableName, join::type type = join::inner);

            /*!
             * sets the join clause for this query
             * @param  value the join clause to set
             * @return       a reference to this
             */
            select_query &join(const join_clause &value);

            /*!
             * converts this query into a sql string
             * @return the sql string
             */
            std::string to_string() const;

            /*!
             * executes this query
             * @return a resultset object
             */
            resultset execute();

            /*!
             * executes this query
             * @param funk a callback to perform on the resultset
             */
            void execute(const std::function<void(const resultset &)> &funk);

            /*!
             * executes this query
             * @return a count of the number of rows
             */
            int count();

            /*!
             * resets this query for re-execution
             */
            void reset();

            /*!
             * gets the table name of this query
             * @return the table name string
             */
            std::string table_name() const;

            /*!
             * sets the table name
             * @param  value the table name to set
             * @return       a reference to this
             */
            select_query &table_name(const std::string &value);

            /*!
             * return the first column in the first row of the result set
             */
            template <typename T>
            T execute_scalar()
            {
                auto rs = execute();

                if (!rs.is_valid() || rs.begin() == rs.end()) return T();

                auto row = rs.begin();

                auto col = row->begin();

                return col->to_value();
            }
        };
    }
}

#endif

/*!
 * @file select_query.h
 * @copyright ryan jennings (ryan-jennings.net), 2013
 */
#ifndef RJ_DB_SELECT_QUERY_H
#define RJ_DB_SELECT_QUERY_H

#include "join_clause.h"
#include "query.h"
#include "resultset.h"
#include "where_clause.h"

namespace rj
{
    namespace db
    {
        struct union_operator;

        namespace union_op
        {
            typedef enum { none, all } type;
        }

        /*!
         * a query to select values from a table
         */
        class select_query : public query
        {
           private:
            where_clause where_;
            std::vector<join_clause> join_;
            std::string limit_;
            std::string orderBy_;
            std::string groupBy_;
            std::vector<std::string> columns_;
            std::string tableName_;
            std::shared_ptr<union_operator> union_;

            select_query &column(const std::string &value)
            {
                columns_.push_back(value);
                return *this;
            }

           public:
            /*!
             * defaults to 'select *'
             * @param db the database in use
             */
            select_query(const std::shared_ptr<rj::db::session> &session);

            /*!
             * @param db        the database in use
             * @param tableName the table name to query
             * @param columns   the columns to query
             */
            select_query(const std::shared_ptr<rj::db::session> &session, const std::vector<std::string> &columns);

            /*!
             * @param schema    the schema to query
             */
            select_query(const std::shared_ptr<schema> &schema);

            /*!
             * @param db  the database in use
             * @param columns the columns to query
             * @param tableName the table to query from
             */
            select_query(const std::shared_ptr<rj::db::session> &session, const std::vector<std::string> &columns, const std::string &tableName);

            /* boilerplate */
            select_query(const select_query &other);
            select_query(select_query &&other);
            virtual ~select_query();
            select_query &operator=(const select_query &other);
            select_query &operator=(select_query &&other);

            /*!
             * sets which table to select from
             * @param  tableName the table name
             * @return           a reference to this instance
             */
            select_query &from(const std::string &tableName);

            /*!
             * gets the select from table name for this query
             * @return the table name
             */
            std::string from() const;

            /*!
             * sets the columns to select
             * @param other a vector of column names
             * @return a reference to this
             */
            select_query &columns(const std::vector<std::string> &other);

            template <typename... List>
            select_query &columns(const std::string &value, const List &... args)
            {
                column(value);
                columns(args...);
                return *this;
            }

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
             * gets the where clause for this query
             * @return the where clause
             */
            where_clause where() const;

            /*!
             * adds a where clause to this query
             * @param  value the sql string
             * @return       a reference to this
             */
            where_clause &where(const std::string &value);

            /*!
             * adds a where clause to this query and binds parameters to it
             * @param value the sql where string
             * @param args the variadic list of bind values
             * @return a reference to this instance
             */
            template <typename... List>
            select_query &where(const std::string &value, const List &... args)
            {
                where(value);
                bind_all(args...);
                return *this;
            }

            /*!
             * adds a where clause to this query
             * @param  value the where clause
             * @return       a reference to this
             */
            select_query &where(const where_clause &value);

            /*!
             * adds a where clause and binds parameters to it
             * @param value the where clause
             * @param args a variadic list of bind values
             * @return a reference to this instance
             */
            template <typename... List>
            select_query &where(const where_clause &value, const List &... args)
            {
                where(value);
                bind_all(args...);
                return *this;
            }

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
             * sets a union query
             * @param  query the query to union with
             * @param  type  the type of union
             * @return       a reference to this instance
             */
            select_query &union_with(const select_query &query, union_op::type type = union_op::none);

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
            long long count();

            /*!
             * resets this query for re-execution
             */
            void reset();

            /*!
             * return the first column in the first row of the result set
             */
            template <typename T>
            T execute_scalar()
            {
                auto rs = execute();

                if (!rs.is_valid()) {
                    return T();
                }

                auto row = rs.begin();

                if (row == rs.end() || !row->is_valid()) {
                    return T();
                }

                auto col = row->begin();

                if (col == row->end() || !col->is_valid()) {
                    return T();
                }

                return col->to_value();
            }
        };

        /*!
         * union's one select query with another
         */
        struct union_operator {
            select_query query;
            union_op::type type;
            union_operator(const select_query &query, union_op::type type = union_op::none) : query(query), type(type)
            {
            }
        };


        /*!
         * output stream operator for a select query
         */
        std::ostream &operator<<(std::ostream &out, const select_query &other);
    }
}

#endif

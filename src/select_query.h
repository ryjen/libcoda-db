/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
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
            select_query(sqldb *db, const std::string &tableName, const std::vector<std::string> &columns);

            select_query(sqldb *db, const std::string &tableName);

            select_query(const std::shared_ptr<schema> &schema);

            select_query(const select_query &other);

            select_query(select_query &&other);

            virtual ~select_query();

            std::vector<std::string> columns() const;

            std::string limit() const;

            std::string order_by() const;

            std::string group_by() const;

            select_query &operator=(const select_query &other);

            select_query &operator=(select_query &&other);

            where_clause &where(const string &value);

            select_query &where(const where_clause &value);

            select_query &limit(const std::string &value);

            select_query &order_by(const std::string &value);

            select_query &group_by(const std::string &value);

            join_clause &join(const std::string &tableName, join::type type = join::inner);

            select_query &join(const join_clause &value);

            std::string to_string() const;

            resultset execute();

            void execute(const std::function<void(const resultset &)> &);

            int count();

            void reset();

            std::string table_name() const;

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

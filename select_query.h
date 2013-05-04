/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_SELECT_QUERY_H_
#define _ARG3_DB_SELECT_QUERY_H_

#include "base_query.h"
#include "resultset.h"

namespace arg3
{
    namespace db
    {
        class sqldb;

        class select_query : public base_query
        {
        public:
            class where_clause
            {
            private:
                vector<where_clause> and_;
                vector<where_clause> or_;
                string value_;

            public:
                where_clause();
                explicit where_clause(const string &value);
                where_clause(const where_clause &other);
                where_clause(where_clause &&other);
                where_clause &operator=(const where_clause &other);
                where_clause &operator=(where_clause &&other);

                virtual ~where_clause();

                string to_string() const;

                explicit operator string();

                where_clause &operator&&(const string &value);
                where_clause &operator&&(const where_clause &value);
                where_clause &operator||(const where_clause &value);
                where_clause &operator||(const string &value);

                bool empty() const;
            };
            friend class resultset;
        private:
            where_clause where_;
            string limit_;
            string orderBy_;
            string groupBy_;
        public:

            select_query(const sqldb &db, const string &tableName, const vector<string> &columns);

            select_query(const sqldb &db, const string &tableName);

            select_query(const select_query &other);

            select_query(select_query &&other);

            virtual ~select_query();

            select_query &operator=(const select_query &other);

            select_query &operator=(select_query &&other);

            select_query &where(const where_clause &value);

            select_query &where(const string &value);

            select_query &limit(const string &value);

            select_query &orderBy(const string &value);

            select_query &groupBy(const string &value);

            string to_string() const;

            resultset execute();

        };


        inline select_query::where_clause Q(const char *str)
        {
            return select_query::where_clause(str);
        }

    }
}

#endif

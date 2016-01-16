#ifndef ARG3_DB_JOIN_CLAUSE_H
#define ARG3_DB_JOIN_CLAUSE_H

#include <unordered_map>
#include <string>

using namespace std;

namespace arg3
{
    namespace db
    {
        namespace join
        {
            typedef enum { inner, left, right, outer } type;
        }

        /*!
         * a utility class aimed at making join statements
         * ex. join("tablename").on("a", "b").and("c", "d");
         */
        class join_clause
        {
           private:
            string tableName_;
            join::type type_;
            unordered_map<string, string> columns_;

           public:
            join_clause();
            explicit join_clause(const string &tableName, join::type joinType = join::inner);
            join_clause(const join_clause &other);
            join_clause(join_clause &&other);
            join_clause &operator=(const join_clause &other);
            join_clause &operator=(join_clause &&other);

            virtual ~join_clause();

            string to_string() const;

            bool empty() const;
            void reset();

            explicit operator string();

            join_clause &on(const string &colA, const string &colB);
            join_clause &on(const pair<string, string> &columns);
        };

        ostream &operator<<(ostream &out, const join_clause &where);

        // user defined literal for expressions like "this = ?"_w && "that = ?"_w
        join_clause operator"" _join(const char *cstr, size_t len);
    }
}

#endif

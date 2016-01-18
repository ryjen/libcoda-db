#ifndef ARG3_DB_JOIN_CLAUSE_H
#define ARG3_DB_JOIN_CLAUSE_H

#include <unordered_map>
#include <string>
#include "where_clause.h"

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
         * ex. join("tablename").on("a", "b").on("c", "d");
         */
        class join_clause
        {
           private:
            std::string tableName_;
            join::type type_;
            where_clause on_;

           public:
            join_clause();
            explicit join_clause(const std::string &tableName, join::type joinType = join::inner);
            join_clause(const join_clause &other);
            join_clause(join_clause &&other);
            join_clause &operator=(const join_clause &other);
            join_clause &operator=(join_clause &&other);

            virtual ~join_clause();

            std::string to_string() const;

            bool empty() const;
            void reset();

            join_clause &set_type(join::type value);
            join_clause &set_table_name(const std::string &value);
            where_clause &where(const std::string &value);
            join_clause &where(const where_clause &value);

            explicit operator std::string();
        };

        std::ostream &operator<<(std::ostream &out, const join_clause &where);

        // user defined literal for expressions like "this = ?"_w && "that = ?"_w
        join_clause operator"" _join(const char *cstr, size_t len);
    }
}

#endif

#ifndef ARG3_DB_WHERE_CLAUSE_H
#define ARG3_DB_WHERE_CLAUSE_H

#include <vector>
#include <string>

#include "sqldb.h"

namespace arg3
{
    namespace db
    {
        /*!
         * a utility class aimed at making logic where statements
         * ex. where_clause("a = b") || "c == d" && "e == f";
         */
        class where_clause
        {
           private:
            std::string value_;
            std::vector<where_clause> and_;
            std::vector<where_clause> or_;

           public:
            where_clause();
            explicit where_clause(const std::string &value);
            where_clause(const where_clause &other);
            where_clause(where_clause &&other);
            where_clause &operator=(const where_clause &other);
            where_clause &operator=(where_clause &&other);

            virtual ~where_clause();

            virtual std::string to_string() const;

            explicit operator std::string();

            where_clause &operator&&(const std::string &value);
            where_clause &operator&&(const where_clause &value);
            where_clause &operator||(const where_clause &value);
            where_clause &operator||(const std::string &value);

            where_clause &eq(const std::string &col);
            where_clause &eq(const std::string &col, const sql_value &value);

            bool empty() const;

            void reset();
        };

        std::ostream &operator<<(std::ostream &out, const where_clause &where);

        where_clause operator"" _where(const char *cstr, size_t len);

        inline where_clause where(const std::string &str)
        {
            return where_clause(str);
        }
    }
}

#endif

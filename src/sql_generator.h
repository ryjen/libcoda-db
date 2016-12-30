#ifndef RJ_DB_SQL_GENERATOR_H
#define RJ_DB_SQL_GENERATOR_H

#include <string>

namespace rj
{
    namespace db
    {
        class sql_generator
        {
           public:
            sql_generator() = default;
            sql_generator(const sql_generator &other) = default;
            sql_generator(sql_generator &&other) = default;
            sql_generator &operator=(const sql_generator &other) = default;
            sql_generator &operator=(sql_generator &&other) = default;
            virtual ~sql_generator() = default;

            std::string to_sql() const;
            std::string to_sql();

            virtual void reset();

           protected:
            virtual std::string generate_sql() const = 0;

           private:
            std::string sql_;
        };
    }
}
#endif

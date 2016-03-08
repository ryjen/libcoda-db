/*!
 * @file statement.h
 * Mysql specific implementation of a query statement
 */
#ifndef ARG3_DB_MYSQL_STATEMENT_H
#define ARG3_DB_MYSQL_STATEMENT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBMYSQLCLIENT

#include <mysql/mysql.h>
#include "../statement.h"
#include "binding.h"

namespace arg3
{
    namespace db
    {
        namespace mysql
        {
            class session;

            /*!
             * a mysql specific implementation of a statement
             */
            class statement : public arg3::db::statement
            {
               private:
                std::shared_ptr<session> sess_;
                std::shared_ptr<MYSQL_STMT> stmt_;
                binding bindings_;

               public:
                /*!
                 * @param db the database in use
                 */
                statement(const std::shared_ptr<session> &sess);

                /* non-copyable boilerplate */
                statement(const statement &other) = delete;
                statement(statement &&other);
                statement &operator=(const statement &other) = delete;
                statement &operator=(statement &&other);
                virtual ~statement();

                /* statement overrides */
                void prepare(const std::string &sql);
                bool is_valid() const;
                resultset_type results();
                bool result();
                void finish();
                void reset();
                int last_number_of_changes();
                long long last_insert_id();
                std::string last_error();

                /* bindable overrides */
                statement &bind(size_t index, int value);
                statement &bind(size_t index, unsigned value);
                statement &bind(size_t index, long long value);
                statement &bind(size_t index, unsigned long long value);
                statement &bind(size_t index, float value);
                statement &bind(size_t index, double value);
                statement &bind(size_t index, const std::string &value, int len = -1);
                statement &bind(size_t index, const std::wstring &value, int len = -1);
                statement &bind(size_t index, const sql_blob &value);
                statement &bind(size_t index, const sql_null_type &value);
                statement &bind(size_t index, const sql_time &value);
                statement &bind(const std::string &name, const sql_value &value);
            };
        }
    }
}

#endif

#endif

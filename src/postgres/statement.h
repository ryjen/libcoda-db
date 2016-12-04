/*!
 * @file statement.h
 * A postgres statement
 */
#ifndef RJ_DB_POSTGRES_STATEMENT_H
#define RJ_DB_POSTGRES_STATEMENT_H

#include <libpq-fe.h>
#include "../statement.h"
#include "binding.h"

namespace rj
{
    namespace db
    {
        namespace postgres
        {
            class session;

            /*!
             * a sqlite specific implementation of a statement
             */
            class statement : public rj::db::statement
            {
               private:
                std::shared_ptr<postgres::session> sess_;
                std::shared_ptr<PGresult> stmt_;
                binding bindings_;
                std::string sql_;

               public:
                /*!
                 * @param db    the database in use
                 */
                statement(const std::shared_ptr<postgres::session> &sess);

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
                std::string last_error();
                long long last_insert_id();

                /* bindable overrides */
                statement &bind(size_t index, const sql_value &value);
                statement &bind(const std::string &name, const sql_value &value);
                size_t num_of_bindings() const;
            };


            /*
             * utility to cleanup a postgres result
             */
            namespace helper
            {
                struct res_delete {
                    void operator()(PGresult *p) const;
                };
            }
        }
    }
}

#endif

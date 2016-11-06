/*!
 * a sqlite3 sql statement
 * @file statement.h
 */
#ifndef RJ_DB_SQLITE_STATEMENT_H
#define RJ_DB_SQLITE_STATEMENT_H

#include <sqlite3.h>
#include "../statement.h"

namespace rj
{
    namespace db
    {
        namespace sqlite
        {
            class session;

            /*!
             * a sqlite specific implementation of a statement
             */
            class statement : public rj::db::statement
            {
               private:
                std::shared_ptr<sqlite::session> sess_;
                std::shared_ptr<sqlite3_stmt> stmt_;
                size_t bound_;

               public:
                /*!
                 * @param db    the database in use
                 */
                statement(const std::shared_ptr<sqlite::session> &sess);

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

            namespace helper
            {
                /*!
                 * helper to cleanup a sqlite3 statement
                 */
                struct stmt_delete {
                    void operator()(sqlite3_stmt *p) const;
                };
            }
        }
    }
}

#endif

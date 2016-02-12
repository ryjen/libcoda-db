/*!
 * a sqlite3 sql statement
 * @file statement.h
 */
#ifndef ARG3_DB_SQLITE_STATEMENT_H
#define ARG3_DB_SQLITE_STATEMENT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBSQLITE3

#include <sqlite3.h>
#include "../statement.h"

namespace arg3
{
    namespace db
    {
        namespace sqlite
        {
            class db;

            /*!
             * a sqlite specific implementation of a statement
             */
            class statement : public arg3::db::statement
            {
               private:
                sqlite::db *db_;
                std::shared_ptr<sqlite3_stmt> stmt_;

               public:
                /*!
                 * @param db    the database in use
                 */
                statement(sqlite::db *db);

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

#endif

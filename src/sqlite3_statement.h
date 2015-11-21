#ifndef ARG3_DB_SQLITE_STATEMENT_H
#define ARG3_DB_SQLITE_STATEMENT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBSQLITE3

#include <sqlite3.h>
#include "statement.h"

namespace arg3
{
    namespace db
    {
        class sqlite3_db;

        /*!
         * a sqlite specific implementation of a statement
         */
        class sqlite3_statement : public statement
        {
           private:
            sqlite3_db *db_;
            shared_ptr<sqlite3_stmt> stmt_;

           public:
            sqlite3_statement(sqlite3_db *db);
            sqlite3_statement(const sqlite3_statement &other) = delete;
            sqlite3_statement(sqlite3_statement &&other);
            sqlite3_statement &operator=(const sqlite3_statement &other) = delete;
            sqlite3_statement &operator=(sqlite3_statement &&other);
            virtual ~sqlite3_statement();
            void prepare(const std::string &sql);
            bool is_valid() const;
            resultset results();
            bool result();
            void finish();
            void reset();
            int last_number_of_changes();
            string last_error();
            long long last_insert_id();
            sqlite3_statement &bind(size_t index, int value);
            sqlite3_statement &bind(size_t index, long long value);
            sqlite3_statement &bind(size_t index, double value);
            sqlite3_statement &bind(size_t index, const std::string &value, int len = -1);
            sqlite3_statement &bind(size_t index, const sql_blob &value);
            sqlite3_statement &bind(size_t index, const sql_null_type &value);
            sqlite3_statement &bind_value(size_t index, const sql_value &v);
            sqlite3_statement &bind(size_t index, const void *data, size_t size, void (*pFree)(void *));
        };

        struct sqlite3_stmt_delete {
            void operator()(sqlite3_stmt *p) const;
        };
    }
}

#endif

#endif

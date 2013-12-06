#ifndef ARG3_DB_SQLITE_STATEMENT_H_
#define ARG3_DB_SQLITE_STATEMENT_H_

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
            sqlite3_stmt *stmt_;
        public:
            sqlite3_statement(sqlite3_db *db);
            sqlite3_statement(const sqlite3_statement &other) = default;
            sqlite3_statement(sqlite3_statement &&other) = default;
            sqlite3_statement &operator=(const sqlite3_statement &other) = default;
            sqlite3_statement &operator=(sqlite3_statement && other) = default;
            virtual ~sqlite3_statement() = default;
            void prepare(const std::string &sql);
            bool is_valid() const;
            resultset results();
            bool result();
            void finish();
            void reset();
            sqlite3_statement &bind(size_t index, int value);
            sqlite3_statement &bind(size_t index, int64_t value);
            sqlite3_statement &bind(size_t index, double value);
            sqlite3_statement &bind(size_t index, const std::string &value, int len = -1);
            sqlite3_statement &bind(size_t index, const sql_blob &value);
            sqlite3_statement &bind(size_t index, const sql_null_t &value);
            sqlite3_statement &bind_value(size_t index, const sql_value &v);
            sqlite3_statement &bind(size_t index, const void *data, size_t size, void(* pFree)(void *) = SQLITE_STATIC);
        };
    }
}

#endif

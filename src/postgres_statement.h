#ifndef ARG3_DB_POSTGRES_STATEMENT_H
#define ARG3_DB_POSTGRES_STATEMENT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include <libpq-fe.h>
#include "statement.h"
#include "postgres_binding.h"

namespace arg3
{
    namespace db
    {
        class postgres_db;

        /*!
         * a sqlite specific implementation of a statement
         */
        class postgres_statement : public statement
        {
           private:
            postgres_db *db_;
            std::shared_ptr<PGresult> stmt_;
            postgres_binding bindings_;
            std::string sql_;

           public:
            postgres_statement(postgres_db *db);
            postgres_statement(const postgres_statement &other) = delete;
            postgres_statement(postgres_statement &&other);
            postgres_statement &operator=(const postgres_statement &other) = delete;
            postgres_statement &operator=(postgres_statement &&other);
            virtual ~postgres_statement();
            void prepare(const std::string &sql);
            bool is_valid() const;
            resultset results();
            bool result();
            void finish();
            void reset();
            int last_number_of_changes();
            std::string last_error();
            long long last_insert_id();
            postgres_statement &bind(size_t index, int value);
            postgres_statement &bind(size_t index, long long value);
            postgres_statement &bind(size_t index, double value);
            postgres_statement &bind(size_t index, const std::string &value, int len = -1);
            postgres_statement &bind(size_t index, const std::wstring &value, int len = -1);
            postgres_statement &bind(size_t index, const sql_blob &value);
            postgres_statement &bind(size_t index, const sql_null_t &value);
        };
    }
}

#endif

#endif

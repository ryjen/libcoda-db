#ifndef ARG3_DB_MYSQL_STATEMENT_H_
#define ARG3_DB_MYSQL_STATEMENT_H_

#include "config.h"

#ifdef HAVE_LIBMYSQLCLIENT

#include <mysql/mysql.h>

#include "statement.h"

namespace arg3
{
    namespace db
    {

        class mysql_db;

        /*!
         * a mysql specific implementation of a statement
         */
        class mysql_statement : public statement
        {
        private:
            mysql_db *db_;
            MYSQL_BIND *bindings_;
            MYSQL_STMT *stmt_;
            size_t bindingSize_;
            //unsigned *refcount_;
            void reallocate_bindings(size_t index);
            void bind_params();
        public:
            mysql_statement(mysql_db *db);
            mysql_statement(const mysql_statement &other) = delete;
            mysql_statement(mysql_statement &&other);
            mysql_statement &operator=(const mysql_statement &other) = delete;
            mysql_statement &operator=(mysql_statement && other);
            virtual ~mysql_statement();
            void prepare(const std::string &sql);
            bool is_valid() const;
            resultset results();
            bool result();
            void finish();
            void reset();
            int last_number_of_changes();
            string last_error();
            mysql_statement &bind(size_t index, int value);
            mysql_statement &bind(size_t index, int64_t value);
            mysql_statement &bind(size_t index, double value);
            mysql_statement &bind(size_t index, const std::string &value, int len = -1);
            mysql_statement &bind(size_t index, const sql_blob &value);
            mysql_statement &bind(size_t index, const sql_null_t &value);
            mysql_statement &bind_value(size_t index, const sql_value &v);
            mysql_statement &bind(size_t index, const void *data, size_t size, void(* pFree)(void *) = SQLITE_STATIC);
        };
    }
}

#endif

#endif

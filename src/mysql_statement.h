#ifndef ARG3_DB_MYSQL_STATEMENT_H
#define ARG3_DB_MYSQL_STATEMENT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBMYSQLCLIENT

#include <mysql/mysql.h>
#include "statement.h"
#include "mysql_binding.h"

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
            shared_ptr<MYSQL_STMT> stmt_;
            mysql_binding bindings_;

           public:
            mysql_statement(mysql_db *db);
            mysql_statement(const mysql_statement &other) = delete;
            mysql_statement(mysql_statement &&other);
            mysql_statement &operator=(const mysql_statement &other) = delete;
            mysql_statement &operator=(mysql_statement &&other);
            virtual ~mysql_statement();
            void prepare(const std::string &sql);
            bool is_valid() const;
            resultset results();
            bool result();
            void finish();
            void reset();
            int last_number_of_changes();
            long long last_insert_id();
            string last_error();

            mysql_statement &bind(size_t index, int value);
            mysql_statement &bind(size_t index, long long value);
            mysql_statement &bind(size_t index, double value);
            mysql_statement &bind(size_t index, const std::string &value, int len = -1);
            mysql_statement &bind(size_t index, const std::wstring &value, int len = -1);
            mysql_statement &bind(size_t index, const sql_blob &value);
            mysql_statement &bind(size_t index, const sql_null_t &value);
        };
    }
}

#endif

#endif

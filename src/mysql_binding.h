#ifndef ARG3_DB_MYSQL_BINDING_H_
#define ARG3_DB_MYSQL_BINDING_H_

#include "config.h"

#ifdef HAVE_LIBMYSQLCLIENT

#include "sql_value.h"
#include "bindable.h"
#include <mysql/mysql.h>
#include <string>

namespace arg3
{
    namespace db
    {
        class sql_value;
        class sql_blob;

        class mysql_binding : public bindable
        {
            friend class mysql_column;
        private:
            MYSQL_BIND *value_;
            size_t size_;
            void copy_value(const MYSQL_BIND *other, size_t size);
            void clear_value();
            void clear_value(size_t index);
            void reallocate_value(size_t index);
        public:
            mysql_binding();
            mysql_binding(size_t size);
            mysql_binding(const MYSQL_BIND &value);
            mysql_binding(MYSQL_BIND *value, size_t size);
            mysql_binding(MYSQL_FIELD *fields, size_t size);
            mysql_binding(const mysql_binding &other);
            mysql_binding(mysql_binding &&other);
            mysql_binding &operator=(const mysql_binding &other);
            mysql_binding &operator=(mysql_binding && other);
            virtual ~mysql_binding();

            void bind_result(MYSQL_STMT *stmt) const;

            size_t size() const;

            MYSQL_BIND *get(size_t index) const;

            sql_blob to_blob(size_t index) const;

            double to_double(size_t index, const double def = DOUBLE_DEFAULT) const;

            int to_int(size_t index, const int def = INT_DEFAULT) const;

            bool to_bool(size_t index, const bool def = BOOL_DEFAULT) const;

            long long to_llong(size_t index, const long long def = INT_DEFAULT) const;

            std::string to_string(size_t index) const;

            sql_value to_value(size_t index) const;

            time_t to_timestamp(size_t index, const time_t def = INT_DEFAULT) const;

            int type(size_t index) const;

            mysql_binding &bind(size_t index, int value);
            mysql_binding &bind(size_t index, long long value);
            mysql_binding &bind(size_t index, double value);
            mysql_binding &bind(size_t index, const std::string &value, int len = -1);
            mysql_binding &bind(size_t index, const sql_blob &value);
            mysql_binding &bind(size_t index, const sql_null_type &value);
            mysql_binding &bind_value(size_t index, const sql_value &v);
            mysql_binding &bind(size_t index, const void *data, size_t size, void(* pFree)(void *));

            void bind_params(MYSQL_STMT *stmt) const;

            void reset();
        };

    }
}

#endif


#endif

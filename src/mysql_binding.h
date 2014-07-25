#ifndef ARG3_DB_MYSQL_BINDING_H_
#define ARG3_DB_MYSQL_BINDING_H_

#include "config.h"

#ifdef HAVE_LIBMYSQLCLIENT

#include "sql_value.h"
#include <mysql/mysql.h>
#include <string>

namespace arg3
{
    namespace db
    {
        class sql_value;
        class sql_blob;

        class mysql_binding
        {
            friend class mysql_column;
        private:
            MYSQL_BIND *value_;
            size_t size_;
            void copy_value(const MYSQL_BIND *other, size_t size);
            void clear_value();
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

            void bind_result(MYSQL_STMT *stmt);

            std::shared_ptr<mysql_binding> get(size_t index) const;

            sql_blob to_blob(size_t index) const;

            double to_double(size_t index) const;

            int to_int(size_t index) const;

            bool to_bool(size_t index) const;

            int64_t to_int64(size_t index) const;

            std::string to_string(size_t index) const;

            sql_value to_value(size_t index) const;

            int type(size_t index) const;
        };

    }
}

#endif


#endif

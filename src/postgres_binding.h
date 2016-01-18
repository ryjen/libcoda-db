#ifndef ARG3_DB_POSTGRES_BINDING_H
#define ARG3_DB_POSTGRES_BINDING_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include "sql_value.h"
#include "bindable.h"
#include <libpq-fe.h>
#include <string>

namespace arg3
{
    namespace db
    {
        class sql_value;

        class postgres_binding : public bindable
        {
            friend class postgres_column;
            friend class postgres_statement;

           private:
            char **values_;
            Oid *types_;
            int *lengths_;
            int *formats_;
            size_t size_;
            void copy_value(const postgres_binding &other);
            void clear_value();
            void clear_value(size_t index);
            bool reallocate_value(size_t index);

           public:
            postgres_binding();
            postgres_binding(size_t size);
            postgres_binding(const postgres_binding &other);
            postgres_binding(postgres_binding &&other);
            postgres_binding &operator=(const postgres_binding &other);
            postgres_binding &operator=(postgres_binding &&other);
            virtual ~postgres_binding();

            size_t size() const;

            sql_value to_value(size_t index) const;

            int sql_type(size_t index) const;

            postgres_binding &bind(size_t index, int value);
            postgres_binding &bind(size_t index, long long value);
            postgres_binding &bind(size_t index, double value);
            postgres_binding &bind(size_t index, const std::string &value, int len = -1);
            postgres_binding &bind(size_t index, const std::wstring &value, int len = -1);
            postgres_binding &bind(size_t index, const sql_blob &value);
            postgres_binding &bind(size_t index, const sql_null_t &value);

            void reset();
        };
    }
}

#endif


#endif

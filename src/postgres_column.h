#ifndef ARG3_DB_POSTGRES_COLUMN_H
#define ARG3_DB_POSTGRES_COLUMN_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include <libpq-fe.h>
#include "column.h"

#define postgres_default_column postgres_cached_column

namespace arg3
{
    namespace db
    {
        /*!
         * a sqlite specific implementation of a column
         */
        class postgres_column : public column_impl
        {
           private:
            std::shared_ptr<PGresult> stmt_;
            int column_;
            int row_;

           public:
            postgres_column(const std::shared_ptr<PGresult> &stmt, int row, int column);
            postgres_column(const postgres_column &other) = delete;
            postgres_column(postgres_column &&other);
            virtual ~postgres_column();
            postgres_column &operator=(const postgres_column &other) = delete;
            postgres_column &operator=(postgres_column &&other);

            bool is_valid() const;

            sql_value to_value() const;

            int sql_type() const;

            std::string name() const;
        };

        /*!
         * a column that contains pre-fetched values
         */
        class postgres_cached_column : public column_impl
        {
           private:
            std::string name_;
            sql_value value_;
            int length_;
            int type_;
            void set_value(const std::shared_ptr<PGresult> &stmt, size_t row, size_t column);

           public:
            postgres_cached_column(const std::shared_ptr<PGresult> &stmt, size_t row, size_t column);
            postgres_cached_column(const postgres_cached_column &other) = delete;
            postgres_cached_column(postgres_cached_column &&other);
            virtual ~postgres_cached_column();
            postgres_cached_column &operator=(const postgres_cached_column &other) = delete;
            postgres_cached_column &operator=(postgres_cached_column &&other);

            bool is_valid() const;

            sql_value to_value() const;

            int sql_type() const;

            std::string name() const;
        };
    }
}

#endif

#endif

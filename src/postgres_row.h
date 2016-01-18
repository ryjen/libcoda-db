#ifndef ARG3_DB_POSTGRES_ROW_H
#define ARG3_DB_POSTGRES_ROW_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include "row.h"
#include "postgres_column.h"
#include <vector>

#define postgres_default_row arg3::db::postgres_cached_row

namespace arg3
{
    namespace db
    {
        class postgres_db;

        /*!
         *  a sqlite specific implementation of a row
         */
        class postgres_row : public row_impl
        {
            friend class postgres_resultset;

           private:
            std::shared_ptr<PGresult> stmt_;
            postgres_db *db_;
            size_t size_;
            size_t row_;

           public:
            postgres_row(postgres_db *db, std::shared_ptr<PGresult> stmt, size_t row);
            virtual ~postgres_row();
            postgres_row(const postgres_row &other) = delete;
            postgres_row(postgres_row &&other);
            postgres_row &operator=(const postgres_row &other) = delete;
            postgres_row &operator=(postgres_row &&other);

            std::string column_name(size_t nPosition) const;
            column_type column(size_t nPosition) const;
            column_type column(const std::string &name) const;
            size_t size() const;
            bool is_valid() const;
        };

        /*!
         * a row that contains pre-fetched columns
         */
        class postgres_cached_row : public row_impl
        {
            friend class postgres_resultset;

           private:
            std::vector<std::shared_ptr<postgres_cached_column>> columns_;

           public:
            postgres_cached_row(postgres_db *db, std::shared_ptr<PGresult> stmt, size_t row);
            virtual ~postgres_cached_row();
            postgres_cached_row(const postgres_cached_row &other) = delete;
            postgres_cached_row(postgres_cached_row &&other);
            postgres_cached_row &operator=(const postgres_cached_row &other) = delete;
            postgres_cached_row &operator=(postgres_cached_row &&other);

            std::string column_name(size_t nPosition) const;
            column_type column(size_t nPosition) const;
            column_type column(const std::string &name) const;
            size_t size() const;
            bool is_valid() const;
        };
    }
}

#endif

#endif

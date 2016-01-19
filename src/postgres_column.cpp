#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include "postgres_column.h"

namespace arg3
{
    namespace db
    {
        postgres_column::postgres_column(shared_ptr<PGresult> stmt, size_t row, size_t column) : stmt_(stmt), column_(column), row_(row)
        {
        }

        postgres_column::postgres_column(postgres_column &&other) : stmt_(other.stmt_), column_(other.column_), row_(other.row_)
        {
            other.stmt_ = nullptr;
        }

        postgres_column::~postgres_column()
        {
            stmt_ = nullptr;
        }

        postgres_column &postgres_column::operator=(postgres_column &&other)
        {
            stmt_ = other.stmt_;
            row_ = other.row_;
            column_ = other.column_;
            other.stmt_ = nullptr;
            return *this;
        }

        bool postgres_column::is_valid() const
        {
            return stmt_ != nullptr;
        }

        sql_value postgres_column::to_value() const
        {
            if (!is_valid()) {
                throw no_such_column_exception();
            }

            return PQgetvalue(stmt_.get(), row_, column_);
        }

        int postgres_column::sql_type() const
        {
            if (!is_valid()) {
                throw no_such_column_exception();
            }
            return PQftype(stmt_.get(), column_);
        }

        string postgres_column::name() const
        {
            return PQfname(stmt_.get(), column_);
        }


        /* cached version */

        postgres_cached_column::postgres_cached_column(shared_ptr<PGresult> stmt, size_t row, size_t column)
        {
            set_value(stmt, row, column);
        }

        void postgres_cached_column::set_value(shared_ptr<PGresult> stmt, size_t row, size_t column)
        {
            if (stmt == nullptr) {
                throw database_exception("no statement provided to postgres column");
            }
            name_ = PQfname(stmt.get(), column);
            type_ = PQftype(stmt.get(), column);
            value_ = PQgetvalue(stmt.get(), row, column);
        }


        postgres_cached_column::postgres_cached_column(postgres_cached_column &&other)
            : name_(std::move(other.name_)), value_(std::move(other.value_)), type_(other.type_)
        {
        }

        postgres_cached_column::~postgres_cached_column()
        {
        }

        postgres_cached_column &postgres_cached_column::operator=(postgres_cached_column &&other)
        {
            name_ = std::move(other.name_);
            type_ = other.type_;
            value_ = std::move(other.value_);

            return *this;
        }

        bool postgres_cached_column::is_valid() const
        {
            return true;
        }

        sql_value postgres_cached_column::to_value() const
        {
            return value_;
        }

        int postgres_cached_column::sql_type() const
        {
            return type_;
        }

        string postgres_cached_column::name() const
        {
            return name_;
        }
    }
}

#endif

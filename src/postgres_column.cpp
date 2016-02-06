#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include "postgres_column.h"

namespace arg3
{
    namespace db
    {
        namespace helper
        {
            sql_value convert_raw_value(const char *, Oid, int);
        }

        postgres_column::postgres_column(const shared_ptr<PGresult> &stmt, int row, int column) : stmt_(stmt), column_(column), row_(row)

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
            return stmt_ != nullptr && row_ >= 0 && column_ >= 0;
        }

        sql_value postgres_column::to_value() const
        {
            if (!is_valid()) {
                throw no_such_column_exception();
            }

            return helper::convert_raw_value(PQgetvalue(stmt_.get(), row_, column_), PQftype(stmt_.get(), column_),
                                             PQgetlength(stmt_.get(), row_, column_));
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
    }
}

#endif

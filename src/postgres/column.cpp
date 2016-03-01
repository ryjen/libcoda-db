#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include "column.h"
#include "binding.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        namespace postgres
        {
            column::column(const shared_ptr<PGresult> &stmt, int row, int column) : stmt_(stmt), column_(column), row_(row)

            {
            }

            column::column(column &&other) : stmt_(other.stmt_), column_(other.column_), row_(other.row_)
            {
                other.stmt_ = nullptr;
            }

            column::~column()
            {
            }

            column &column::operator=(column &&other)
            {
                stmt_ = other.stmt_;
                row_ = other.row_;
                column_ = other.column_;
                other.stmt_ = nullptr;
                return *this;
            }

            bool column::is_valid() const
            {
                return stmt_ != nullptr && row_ >= 0 && column_ >= 0;
            }

            sql_value column::to_value() const
            {
                if (!is_valid()) {
                    throw no_such_column_exception();
                }

                return data_mapper::to_value(PQftype(stmt_.get(), column_), PQgetvalue(stmt_.get(), row_, column_),
                                             PQgetlength(stmt_.get(), row_, column_));
            }

            int column::sql_type() const
            {
                if (!is_valid()) {
                    throw no_such_column_exception();
                }
                return PQftype(stmt_.get(), column_);
            }

            string column::name() const
            {
                return PQfname(stmt_.get(), column_);
            }
        }
    }
}

#endif

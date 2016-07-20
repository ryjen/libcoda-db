#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBSQLITE3

#include "column.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        namespace sqlite
        {
            namespace data_mapper
            {
                sql_value to_value(const shared_ptr<sqlite3_stmt> &stmt, int column)
                {
                    if (stmt == nullptr || column < 0 || column >= sqlite3_column_count(stmt.get())) {
                        return sql_value();
                    }

                    switch (sqlite3_column_type(stmt.get(), column)) {
                        case SQLITE_INTEGER:
                            return sqlite3_column_int64(stmt.get(), column);
                        case SQLITE3_TEXT:
                        default: {
                            const unsigned char *textValue = sqlite3_column_text(stmt.get(), column);
                            if (textValue != NULL) {
                                return reinterpret_cast<const char *>(textValue);
                            }
                            return sql_value();
                        }
                        case SQLITE_FLOAT:
                            return sqlite3_column_double(stmt.get(), column);
                        case SQLITE_BLOB:
                            return sql_blob(sqlite3_column_blob(stmt.get(), column), sqlite3_column_bytes(stmt.get(), column));
                    }
                }
            }

            column::column(const shared_ptr<sqlite3_stmt> &stmt, int column) : stmt_(stmt), column_(column)
            {
            }

            column::column(column &&other) : stmt_(other.stmt_), column_(other.column_)
            {
                other.stmt_ = nullptr;
            }

            column::~column()
            {
            }

            column &column::operator=(column &&other)
            {
                stmt_ = other.stmt_;
                column_ = other.column_;

                other.stmt_ = nullptr;

                return *this;
            }

            bool column::is_valid() const
            {
                return stmt_ != nullptr && stmt_ && column_ >= 0;
            }

            sql_value column::to_value() const
            {
                if (!is_valid()) {
                    throw no_such_column_exception();
                }

                return data_mapper::to_value(stmt_, column_);
            }

            int column::sql_type() const
            {
                if (!is_valid()) {
                    throw no_such_column_exception();
                }
                return sqlite3_column_type(stmt_.get(), column_);
            }

            string column::name() const
            {
                if (!is_valid()) {
                    return string();
                }
                return sqlite3_column_name(stmt_.get(), column_);
            }
        }
    }
}

#endif

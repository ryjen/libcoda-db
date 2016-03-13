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


            /* cached version */

            cached_column::cached_column(const shared_ptr<sqlite3_stmt> &stmt, int column)
            {
                set_value(stmt, column);
            }

            void cached_column::set_value(const shared_ptr<sqlite3_stmt> &stmt, int column)
            {
                if (stmt == nullptr) {
                    throw database_exception("no statement provided to sqlite3 column");
                }

                if (column < 0 || column >= sqlite3_column_count(stmt.get())) {
                    throw no_such_column_exception();
                }

                name_ = sqlite3_column_name(stmt.get(), column);
                type_ = sqlite3_column_type(stmt.get(), column);
                value_ = data_mapper::to_value(stmt, column);
            }


            cached_column::cached_column(cached_column &&other) : name_(std::move(other.name_)), value_(std::move(other.value_)), type_(other.type_)
            {
                other.type_ = -1;
            }

            cached_column::~cached_column()
            {
            }

            cached_column &cached_column::operator=(cached_column &&other)
            {
                name_ = std::move(other.name_);
                type_ = other.type_;
                value_ = std::move(other.value_);

                other.type_ = -1;
                return *this;
            }

            bool cached_column::is_valid() const
            {
                return type_ > -1;
            }

            sql_value cached_column::to_value() const
            {
                return value_;
            }

            int cached_column::sql_type() const
            {
                return type_;
            }

            string cached_column::name() const
            {
                return name_;
            }
        }
    }
}

#endif

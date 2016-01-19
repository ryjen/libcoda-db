#ifndef ARG3_DB_SQLITE_COLUMN_H
#define ARG3_DB_SQLITE_COLUMN_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBSQLITE3

#include <sqlite3.h>
#include "column.h"

#define sqlite3_default_column sqlite3_cached_column

namespace arg3
{
    namespace db
    {
        /*!
         * a sqlite specific implementation of a column
         */
        class sqlite3_column : public column_impl
        {
           private:
            shared_ptr<sqlite3_stmt> stmt_;
            int column_;

           public:
            sqlite3_column(const shared_ptr<sqlite3_stmt> &stmt, int column);
            sqlite3_column(const sqlite3_column &other) = delete;
            sqlite3_column(sqlite3_column &&other);
            virtual ~sqlite3_column();
            sqlite3_column &operator=(const sqlite3_column &other) = delete;
            sqlite3_column &operator=(sqlite3_column &&other);

            bool is_valid() const;

            sql_value to_value() const;

            int sql_type() const;

            std::string name() const;
        };

        /*!
         * a column that contains pre-fetched values
         */
        class sqlite3_cached_column : public column_impl
        {
           private:
            std::string name_;
            sql_value value_;
            int type_;
            void set_value(std::shared_ptr<sqlite3_stmt> stmt, int column);

           public:
            sqlite3_cached_column(shared_ptr<sqlite3_stmt> stmt, int column);
            sqlite3_cached_column(const sqlite3_cached_column &other) = delete;
            sqlite3_cached_column(sqlite3_cached_column &&other);
            virtual ~sqlite3_cached_column();
            sqlite3_cached_column &operator=(const sqlite3_cached_column &other) = delete;
            sqlite3_cached_column &operator=(sqlite3_cached_column &&other);

            bool is_valid() const;

            sql_value to_value() const;

            int sql_type() const;

            std::string name() const;
        };
    }
}

#endif

#endif

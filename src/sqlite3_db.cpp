#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBSQLITE3

#include "sqlite3_db.h"
#include "sqlite3_statement.h"
#include "sqlite3_resultset.h"

namespace arg3
{
    namespace db
    {
        sqlite3_db::sqlite3_db(const uri &info) : sqldb(info), db_(NULL)
        {
        }

        sqlite3_db::sqlite3_db(const sqlite3_db &other) : sqldb(other), db_(other.db_)
        {
        }

        sqlite3_db::sqlite3_db(sqlite3_db &&other) : sqldb(other), db_(std::move(other.db_))
        {
        }

        sqlite3_db &sqlite3_db::operator=(const sqlite3_db &other)
        {
            sqldb::operator=(other);

            db_ = other.db_;

            return *this;
        }

        sqlite3_db &sqlite3_db::operator=(sqlite3_db &&other)
        {
            sqldb::operator=(std::move(other));

            db_ = std::move(other.db_);

            return *this;
        }

        sqlite3_db::~sqlite3_db()
        {
            close();
        }

        void sqlite3_db::query_schema(const string &tableName, std::vector<column_definition> &columns)
        {
            if (tableName.empty()) {
                throw database_exception("no table name to query schema");
            }

            auto rs = execute("pragma table_info(" + tableName + ")");

            for (auto &row : rs) {
                column_definition def;

                // column name
                def.name = row["name"].to_value().to_string();

                // primary key check
                def.pk = row["pk"].to_value().to_bool();

                // find type
                def.type = row["type"].to_value().to_string();

                columns.push_back(def);
            }
        }

        void sqlite3_db::open()
        {
            open(SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI);
        }

        void sqlite3_db::open(int flags)
        {
            if (db_ != NULL) return;

            if (sqlite3_open_v2(connection_info().path.c_str(), &db_, flags, NULL) != SQLITE_OK) throw database_exception(last_error());
        }

        bool sqlite3_db::is_open() const
        {
            return db_ != NULL;
        }

        void sqlite3_db::close()
        {
            if (db_ == NULL) return;

            sqlite3_close(db_);
            db_ = NULL;
        }

        string sqlite3_db::last_error() const
        {
            ostringstream buf;

            buf << sqlite3_errcode(db_);
            buf << ": " << sqlite3_errmsg(db_);

            return buf.str();
        }

        long long sqlite3_db::last_insert_id() const
        {
            return sqlite3_last_insert_rowid(db_);
        }

        int sqlite3_db::last_number_of_changes() const
        {
            return sqlite3_changes(db_);
        }

        resultset sqlite3_db::execute(const string &sql, bool cache)
        {
            sqlite3_stmt *stmt;

            if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
                throw database_exception(last_error());
            }

            shared_ptr<resultset_impl> impl;

            if (cache)
                impl = make_shared<sqlite3_cached_resultset>(this, shared_ptr<sqlite3_stmt>(stmt, sqlite3_stmt_delete()));
            else
                impl = make_shared<sqlite3_resultset>(this, shared_ptr<sqlite3_stmt>(stmt, sqlite3_stmt_delete()));

            resultset set(impl);

            set.next();

            return set;
        }

        shared_ptr<statement> sqlite3_db::create_statement()
        {
            return make_shared<sqlite3_statement>(this);
        }
    }
}

#endif

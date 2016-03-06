#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBSQLITE3

#include "db.h"
#include "statement.h"
#include "resultset.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        namespace sqlite
        {
            namespace helper
            {
                struct close_db {
                    void operator()(sqlite3 *p) const
                    {
                        if (p != nullptr) {
                            sqlite3_close(p);
                        }
                    }
                };
            }
            db::db(const uri &info) : sqldb(info), db_(nullptr), cacheLevel_(cache::None)
            {
            }

            db::db(const db &other) : sqldb(other), db_(other.db_), cacheLevel_(other.cacheLevel_)
            {
            }

            db::db(db &&other) : sqldb(other), db_(std::move(other.db_)), cacheLevel_(other.cacheLevel_)
            {
                other.db_ = nullptr;
            }

            db &db::operator=(const db &other)
            {
                sqldb::operator=(other);

                db_ = other.db_;
                cacheLevel_ = other.cacheLevel_;

                return *this;
            }

            db &db::operator=(db &&other)
            {
                sqldb::operator=(std::move(other));

                db_ = std::move(other.db_);
                cacheLevel_ = other.cacheLevel_;

                other.db_ = nullptr;

                return *this;
            }

            db::~db()
            {
            }

            void db::query_schema(const string &tableName, std::vector<column_definition> &columns)
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

            void db::open()
            {
                open(SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI);
            }

            void db::open(int flags)
            {
                if (db_ != nullptr) {
                    return;
                }

                sqlite3 *conn = nullptr;

                if (sqlite3_open_v2(connection_info().path.c_str(), &conn, flags, nullptr) != SQLITE_OK) {
                    throw database_exception(last_error());
                }

                db_ = shared_ptr<sqlite3>(conn, helper::close_db());
            }

            bool db::is_open() const
            {
                return db_ != nullptr;
            }

            void db::close()
            {
                // the shared_ptr destructor should close
                db_ = nullptr;
            }

            string db::last_error() const
            {
                if (db_ == nullptr) {
                    return string();
                }
                ostringstream buf;

                buf << sqlite3_errcode(db_.get());
                buf << ": " << sqlite3_errmsg(db_.get());

                return buf.str();
            }

            long long db::last_insert_id() const
            {
                if (db_ == nullptr) {
                    return 0;
                }
                return sqlite3_last_insert_rowid(db_.get());
            }

            int db::last_number_of_changes() const
            {
                if (db_ == nullptr) {
                    return 0;
                }
                return sqlite3_changes(db_.get());
            }

            db::resultset_type db::execute(const string &sql)
            {
                sqlite3_stmt *stmt;

                if (db_ == nullptr) {
                    throw database_exception("db::execute database not open");
                }

                if (sqlite3_prepare_v2(db_.get(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                    throw database_exception(last_error());
                }

                shared_ptr<resultset_impl> impl;

                if (cache_level() == cache::ResultSet) {
                    impl = make_shared<cached_resultset>(this, shared_ptr<sqlite3_stmt>(stmt, helper::stmt_delete()));
                } else {
                    impl = make_shared<resultset>(this, shared_ptr<sqlite3_stmt>(stmt, helper::stmt_delete()));
                }

                resultset_type set(impl);

                set.next();

                return set;
            }

            shared_ptr<db::statement_type> db::create_statement()
            {
                return make_shared<statement>(this);
            }

            db &db::cache_level(cache::level level)
            {
                cacheLevel_ = level;
                return *this;
            }

            cache::level db::cache_level() const
            {
                return cacheLevel_;
            }
        }
    }
}

#endif

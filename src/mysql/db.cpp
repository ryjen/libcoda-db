#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBMYSQLCLIENT

#include "db.h"
#include "statement.h"
#include "resultset.h"

namespace arg3
{
    namespace db
    {
        namespace mysql
        {
            namespace helper
            {
                struct close_db {
                    void operator()(MYSQL *p) const
                    {
                        if (p != nullptr) {
                            mysql_close(p);
                        }
                    }
                };

                string last_stmt_error(MYSQL_STMT *stmt)
                {
                    if (!stmt) return "invalid";

                    ostringstream buf;

                    buf << mysql_stmt_errno(stmt);
                    buf << ": " << mysql_stmt_error(stmt);

                    return buf.str();
                }
            }

            db::db(const uri &connInfo) : sqldb(connInfo), db_(nullptr)
            {
            }

            db::db(const db &other) : sqldb(other), db_(other.db_)
            {
            }

            db::db(db &&other) : sqldb(other), db_(other.db_)
            {
                other.db_ = nullptr;
            }

            db &db::operator=(const db &other)
            {
                sqldb::operator=(other);

                db_ = other.db_;

                return *this;
            }

            db &db::operator=(db &&other)
            {
                sqldb::operator=(std::move(other));

                db_ = other.db_;
                other.db_ = nullptr;

                return *this;
            }

            db::~db()
            {
            }

            void db::open()
            {
                if (db_ != nullptr) return;

                MYSQL *conn = mysql_init(nullptr);

                if (conn == NULL) {
                    throw database_exception("out of memory connecting to mysql");
                }

                auto info = connection_info();

                int port;

                try {
                    if (info.port.empty()) {
                        port = 0;
                    } else {
                        port = stoi(info.port);
                    }
                } catch (const std::exception &e) {
                    port = 0;
                }

                if (mysql_real_connect(conn, info.host.c_str(), info.user.c_str(), info.password.c_str(), info.path.c_str(), port, nullptr, 0) ==
                    nullptr) {
                    throw database_exception("No connection could be made to the database");
                }

                db_ = shared_ptr<MYSQL>(conn, helper::close_db());
            }

            bool db::is_open() const
            {
                return db_ != nullptr;
            }

            void db::close()
            {
                if (db_ != nullptr) {
                    db_ = nullptr;
                }
            }

            string db::last_error() const
            {
                if (db_ == nullptr) {
                    return string();
                }

                ostringstream buf;

                buf << mysql_errno(db_.get());
                buf << ": " << mysql_error(db_.get());

                return buf.str();
            }

            long long db::last_insert_id() const
            {
                if (db_ == nullptr) {
                    return 0;
                }

                return mysql_insert_id(db_.get());
            }

            int db::last_number_of_changes() const
            {
                if (db_ == nullptr) {
                    return 0;
                }

                return mysql_affected_rows(db_.get());
            }

            resultset_type db::execute(const string &sql)
            {
                MYSQL_RES *res = nullptr;

                if (db_ == nullptr) {
                    throw database_exception("database is not open");
                }

                if (mysql_real_query(db_.get(), sql.c_str(), sql.length())) {
                    throw database_exception(last_error());
                }

                res = mysql_store_result(db_.get());

                if (res == nullptr && mysql_field_count(db_.get()) != 0) {
                    throw database_exception(last_error());
                }

                return db::resultset(make_shared<resultset>(this, shared_ptr<MYSQL_RES>(res, helper::res_delete())));
            }

            shared_ptr<statement> db::create_statement()
            {
                return make_shared<mysql_statement>(this);
            }
        }
    }
}

#endif

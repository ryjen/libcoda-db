#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBMYSQLCLIENT

#include "mysql_db.h"
#include "mysql_statement.h"
#include "mysql_resultset.h"

namespace arg3
{
    namespace db
    {
        namespace helper
        {
            struct mysql_close_db {
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

        mysql_db::mysql_db(const uri &connInfo) : sqldb(connInfo), db_(nullptr)
        {
        }

        mysql_db::mysql_db(const mysql_db &other) : sqldb(other), db_(other.db_)
        {
        }

        mysql_db::mysql_db(mysql_db &&other) : sqldb(other), db_(other.db_)
        {
            other.db_ = nullptr;
        }

        mysql_db &mysql_db::operator=(const mysql_db &other)
        {
            sqldb::operator=(other);

            db_ = other.db_;

            return *this;
        }

        mysql_db &mysql_db::operator=(mysql_db &&other)
        {
            sqldb::operator=(std::move(other));

            db_ = other.db_;
            other.db_ = nullptr;

            return *this;
        }

        mysql_db::~mysql_db()
        {
        }

        void mysql_db::query_schema(const string &tableName, std::vector<column_definition> &columns)
        {
            if (!is_open()) return;

            auto rs = execute("show columns from " + tableName);

            for (auto &row : rs) {
                column_definition def;

                // column name
                def.name = row["Field"].to_value().to_string();

                if (def.name.empty()) {
                    continue;
                }

                // primary key check
                def.pk = row["Key"].to_value() == "PRI";

                // find type
                def.type = static_cast<char *>(row["Type"].to_value().to_binary().value());

                columns.push_back(def);
            }
        }

        void mysql_db::open()
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

            db_ = shared_ptr<MYSQL>(conn, helper::mysql_close_db());
        }

        bool mysql_db::is_open() const
        {
            return db_ != nullptr;
        }

        void mysql_db::close()
        {
            if (db_ != nullptr) {
                db_ = nullptr;
            }
        }

        string mysql_db::last_error() const
        {
            if (db_ == nullptr) {
                return string();
            }

            ostringstream buf;

            buf << mysql_errno(db_.get());
            buf << ": " << mysql_error(db_.get());

            return buf.str();
        }

        long long mysql_db::last_insert_id() const
        {
            if (db_ == nullptr) {
                return 0;
            }

            return mysql_insert_id(db_.get());
        }

        int mysql_db::last_number_of_changes() const
        {
            if (db_ == nullptr) {
                return 0;
            }

            return mysql_affected_rows(db_.get());
        }

        resultset mysql_db::execute(const string &sql, bool cache)
        {
            if (db_ == nullptr) {
                throw database_exception("database is not open");
            }

            MYSQL_RES *res;

            if (mysql_real_query(db_.get(), sql.c_str(), sql.length())) throw database_exception(last_error());

            res = mysql_store_result(db_.get());

            if (res == nullptr && mysql_field_count(db_.get()) != 0) {
                throw database_exception(last_error());
            }

            if (cache)
                return resultset(make_shared<mysql_cached_resultset>(this, shared_ptr<MYSQL_RES>(res, mysql_res_delete())));
            else
                return resultset(make_shared<mysql_resultset>(this, shared_ptr<MYSQL_RES>(res, mysql_res_delete())));
        }

        shared_ptr<statement> mysql_db::create_statement()
        {
            return make_shared<mysql_statement>(this);
        }
    }
}

#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBMYSQLCLIENT

#include <sstream>
#include "session.h"
#include "statement.h"
#include "resultset.h"
#include "transaction.h"
#include "../select_query.h"

using namespace std;

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
                    if (!stmt) {
                        return "invalid";
                    }

                    ostringstream buf;

                    buf << mysql_stmt_errno(stmt);
                    buf << ": " << mysql_stmt_error(stmt);

                    return buf.str();
                }
            }

            arg3::db::session *factory::create(const uri &uri)
            {
                return new session(uri);
            }

            session::session(const uri &connInfo) : arg3::db::session(connInfo), db_(nullptr), flags_(CACHE)
            {
            }

            session::session(session &&other) : arg3::db::session(std::move(other)), db_(std::move(other.db_)), flags_(other.flags_)
            {
                other.db_ = nullptr;
            }

            session &session::operator=(session &&other)
            {
                arg3::db::session::operator=(std::move(other));

                db_ = std::move(other.db_);
                flags_ = other.flags_;
                other.db_ = nullptr;

                return *this;
            }

            session::~session()
            {
            }

            session &session::flags(int value)
            {
                flags_ = value;
                return *this;
            }

            int session::flags() const
            {
                return flags_;
            }

            void session::open()
            {
                if (db_ != nullptr) {
                    return;
                }

                MYSQL *conn = mysql_init(nullptr);

                if (conn == nullptr) {
                    throw database_exception("out of memory connecting to mysql");
                }

                auto info = connection_info();

                int port = 3306;

                try {
                    if (!info.port.empty()) {
                        port = stoi(info.port);
                    }
                } catch (const std::exception &e) {
                    throw database_exception("unable to parse port " + info.port);
                }

                if (mysql_real_connect(conn, info.host.c_str(), info.user.c_str(), info.password.c_str(), info.path.c_str(), port, nullptr, 0) ==
                    nullptr) {
                    throw database_exception("No connection could be made to the database");
                }

                db_ = shared_ptr<MYSQL>(conn, helper::close_db());
            }

            bool session::is_open() const
            {
                return db_ != nullptr;
            }

            void session::close()
            {
                if (db_ != nullptr) {
                    db_ = nullptr;
                }
            }

            string session::last_error() const
            {
                if (db_ == nullptr) {
                    return string();
                }

                ostringstream buf;

                buf << mysql_errno(db_.get());
                buf << ": " << mysql_error(db_.get());

                return buf.str();
            }

            long long session::last_insert_id() const
            {
                if (db_ == nullptr) {
                    return 0;
                }

                return mysql_insert_id(db_.get());
            }

            int session::last_number_of_changes() const
            {
                if (db_ == nullptr) {
                    return 0;
                }

                return mysql_affected_rows(db_.get());
            }

            arg3::db::session::resultset_type session::query(const string &sql)
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

                return resultset_type(make_shared<resultset>(static_pointer_cast<mysql::session>(shared_from_this()),
                                                             shared_ptr<MYSQL_RES>(res, helper::res_delete())));
            }

            bool session::execute(const string &sql)
            {
                if (db_ == nullptr) {
                    throw database_exception("database is not open");
                }

                return !mysql_real_query(db_.get(), sql.c_str(), sql.length());
            }

            shared_ptr<arg3::db::session::statement_type> session::create_statement()
            {
                return make_shared<statement>(static_pointer_cast<mysql::session>(shared_from_this()));
            }

            arg3::db::session::transaction_type session::create_transaction()
            {
                return arg3::db::session::transaction_type(shared_from_this(), make_shared<mysql::transaction>(db_));
            }
            void session::query_schema(const string &tableName, std::vector<column_definition> &columns)
            {
                if (!is_open()) return;

                select_query pkq(shared_from_this(), {"tc.table_schema, tc.table_name, kc.column_name, c.extra"});

                pkq.from("information_schema.table_constraints tc");

                pkq.join("information_schema.key_column_usage kc").on("kc.table_name = tc.table_name") and ("kc.table_schema = tc.table_schema");

                pkq.join("information_schema.columns c").on("c.table_name = tc.table_name") and ("c.table_schema = tc.table_schema") and
                    ("c.column_name = kc.column_name");

                pkq.where("tc.constraint_type = 'PRIMARY KEY' AND tc.table_name = $1", tableName);

                pkq.order_by("tc.table_schema, tc.table_name, kc.position_in_unique_constraint");

                auto primary_keys = pkq.execute();

                select_query info_schema(shared_from_this(), {"column_name", "data_type"});

                info_schema.from("information_schema.columns");

                info_schema.where("table_name = $1", tableName);

                auto rs = info_schema.execute();

                for (auto &row : rs) {
                    column_definition def;

                    // column name
                    def.name = row["column_name"].to_value().to_string();

                    if (def.name.empty()) {
                        continue;
                    }

                    def.pk = false;
                    def.autoincrement = false;

                    for (auto &pk : primary_keys) {
                        if (pk["column_name"].to_value() == def.name) {
                            def.pk = true;

                            def.autoincrement = pk["extra"].to_value() == "auto_increment";
                        }
                    }

                    // find type
                    def.type = row["data_type"].to_value().to_string();

                    columns.push_back(def);
                }
            }
        }
    }
}

#endif

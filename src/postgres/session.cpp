#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include "session.h"
#include "statement.h"
#include "resultset.h"
#include "../select_query.h"
#include "transaction.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        namespace postgres
        {
            namespace helper
            {
                struct close_db {
                    void operator()(PGconn *p) const
                    {
                        if (p != nullptr) {
                            PQfinish(p);
                        }
                    }
                };
            }

            arg3::db::session *factory::create(const uri &uri)
            {
                return new session(uri);
            }

            session::session(const uri &info) : arg3::db::session(info), db_(nullptr), lastId_(0), lastNumChanges_(0)
            {
            }

            session::session(session &&other)
                : arg3::db::session(std::move(other)), db_(std::move(other.db_)), lastId_(other.lastId_), lastNumChanges_(other.lastNumChanges_)
            {
                other.db_ = nullptr;
            }

            session &session::operator=(session &&other)
            {
                session::operator=(std::move(other));

                db_ = std::move(other.db_);
                lastId_ = other.lastId_;
                lastNumChanges_ = other.lastNumChanges_;
                other.db_ = nullptr;

                return *this;
            }

            session::~session()
            {
            }

            void session::open()
            {
                if (db_ != nullptr) {
                    return;
                }

                PGconn *conn = PQconnectdb(connection_info().value.c_str());

                if (PQstatus(conn) != CONNECTION_OK) {
                    throw database_exception(PQerrorMessage(conn));
                }

                db_ = shared_ptr<PGconn>(conn, helper::close_db());
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

                return PQerrorMessage(db_.get());
            }

            long long session::last_insert_id() const
            {
                return lastId_;
            }

            void session::set_last_insert_id(long long value)
            {
                lastId_ = value;
            }

            int session::last_number_of_changes() const
            {
                return lastNumChanges_;
            }

            void session::set_last_number_of_changes(int value)
            {
                lastNumChanges_ = value;
            }

            arg3::db::session::resultset_type session::query(const string &sql)
            {
                if (db_ == nullptr) {
                    throw database_exception("database is not open");
                }

                PGresult *res = PQexec(db_.get(), sql.c_str());

                if (PQresultStatus(res) != PGRES_TUPLES_OK && PQresultStatus(res) != PGRES_COMMAND_OK) {
                    throw database_exception(last_error());
                }

                return resultset_type(make_shared<resultset>(static_pointer_cast<postgres::session>(shared_from_this()),
                                                             shared_ptr<PGresult>(res, helper::res_delete())));
            }

            bool session::execute(const string &sql)
            {
                if (db_ == nullptr) {
                    throw database_exception("database is not open");
                }

                PGresult *res = PQexec(db_.get(), sql.c_str());

                return PQresultStatus(res) == PGRES_COMMAND_OK || PQresultStatus(res) == PGRES_TUPLES_OK;
            }

            shared_ptr<arg3::db::session::statement_type> session::create_statement()
            {
                return make_shared<statement>(static_pointer_cast<postgres::session>(shared_from_this()));
            }

            arg3::db::session::transaction_type session::create_transaction()
            {
                return arg3::db::session::transaction_type(shared_from_this(), make_shared<postgres::transaction>(db_));
            }

            string session::insert_sql(const std::shared_ptr<schema> &schema, const vector<string> &columns) const
            {
                if (schema == nullptr) {
                    return string();
                }

                ostringstream buf;

                buf << "INSERT INTO " << schema->table_name();

                buf << "(";

                buf << join_csv(columns);

                buf << ") VALUES(";

                buf << join_params(columns, false);

                buf << ")";

                auto keys = schema->primary_keys();

                auto it = keys.begin();

                if (it != keys.end()) {
                    buf << " RETURNING ";

                    while (it < keys.end() - 1) {
                        buf << *it << ",";
                    }

                    buf << *it;
                }

                buf << ";";

                return buf.str();
            }
            void session::query_schema(const string &tableName, std::vector<column_definition> &columns)
            {
                if (!is_open()) return;

                select_query pkq(shared_from_this(), {"tc.table_schema, tc.table_name, kc.column_name, pg_get_serial_sequence('" + tableName +
                                                      "', kc.column_name) as serial"});

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

                    for (auto &pk : primary_keys) {
                        if (pk["column_name"].to_value() == def.name) {
                            def.pk = true;
                        }
                        def.autoincrement = !pk["serial"].to_value().is_null();
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

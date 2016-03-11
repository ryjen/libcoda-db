#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include "session.h"
#include "statement.h"
#include "resultset.h"
#include "transaction.h"
#include "../schema.h"
#include "../select_query.h"

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

            std::shared_ptr<arg3::db::session_impl> factory::create(const uri &uri)
            {
                return std::make_shared<session>(uri);
            }

            session::session(const uri &info) : session_impl(info), db_(nullptr), lastId_(0), lastNumChanges_(0)
            {
            }

            session::session(session &&other)
                : session_impl(std::move(other)), db_(std::move(other.db_)), lastId_(other.lastId_), lastNumChanges_(other.lastNumChanges_)
            {
                other.db_ = nullptr;
            }

            session &session::operator=(session &&other)
            {
                session_impl::operator=(std::move(other));

                db_ = std::move(other.db_);
                lastId_ = other.lastId_;
                lastNumChanges_ = other.lastNumChanges_;
                other.db_ = nullptr;

                return *this;
            }

            session::~session()
            {
                if (is_open()) {
                    close();
                }
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

            std::shared_ptr<resultset_impl> session::query(const string &sql)
            {
                if (db_ == nullptr) {
                    throw database_exception("database is not open");
                }

                PGresult *res = PQexec(db_.get(), sql.c_str());

                if (PQresultStatus(res) != PGRES_TUPLES_OK && PQresultStatus(res) != PGRES_COMMAND_OK) {
                    throw database_exception(last_error());
                }

                return make_shared<resultset>(shared_from_this(), shared_ptr<PGresult>(res, helper::res_delete()));
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

            shared_ptr<transaction_impl> session::create_transaction() const
            {
                return make_shared<postgres::transaction>(db_);
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

                string pk_sql =
                    string("SELECT tc.table_schema, tc.table_name, kc.column_name FROM information_schema.table_constraints tc ") +
                    "JOIN information_schema.key_column_usage kc ON kc.table_name = tc.table_name AND kc.table_schema = tc.table_schema " +
                    "WHERE tc.constraint_type = 'PRIMARY KEY' AND tc.table_name = '" + tableName +
                    "' ORDER BY tc.table_schema, tc.table_name, kc.position_in_unique_constraint";

                string col_sql =
                    string("SELECT column_name, data_type, pg_get_serial_sequence('" + tableName + "', column_name) as serial ") +
                    "FROM information_schema.columns WHERE table_name = '" + tableName + "'";

                auto primary_keys = query(pk_sql);

                auto rs = query(col_sql);

                while (rs->next()) {
                    auto row = rs->current_row();
                    column_definition def;

                    // column name
                    def.name = row["column_name"].to_value().to_string();

                    if (def.name.empty()) {
                        continue;
                    }

                    def.pk = false;
                    def.autoincrement = false;

                    primary_keys->reset();

                    while (primary_keys->next()) {
                        auto pk = primary_keys->current_row();
                        if (pk["column_name"].to_value() == def.name) {
                            def.pk = true;
                            def.autoincrement = !row["serial"].to_value().to_string().empty();
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

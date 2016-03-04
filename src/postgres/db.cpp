#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include "db.h"
#include "statement.h"
#include "resultset.h"
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

            db::db(const uri &info) : sqldb(info), db_(nullptr), lastId_(0), lastNumChanges_(0)
            {
            }

            db::db(const db &other) : sqldb(other), db_(other.db_), lastId_(other.lastId_), lastNumChanges_(other.lastNumChanges_)
            {
            }

            db::db(db &&other) : sqldb(other), db_(other.db_), lastId_(other.lastId_), lastNumChanges_(other.lastNumChanges_)
            {
                other.db_ = nullptr;
            }

            db &db::operator=(const db &other)
            {
                sqldb::operator=(other);

                db_ = other.db_;
                lastId_ = other.lastId_;
                lastNumChanges_ = other.lastNumChanges_;

                return *this;
            }

            db &db::operator=(db &&other)
            {
                sqldb::operator=(std::move(other));

                db_ = other.db_;
                lastId_ = other.lastId_;
                lastNumChanges_ = other.lastNumChanges_;
                other.db_ = nullptr;

                return *this;
            }

            db::~db()
            {
            }

            void db::open()
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

                return PQerrorMessage(db_.get());
            }

            long long db::last_insert_id() const
            {
                return lastId_;
            }

            void db::set_last_insert_id(long long value)
            {
                lastId_ = value;
            }

            int db::last_number_of_changes() const
            {
                return lastNumChanges_;
            }

            void db::set_last_number_of_changes(int value)
            {
                lastNumChanges_ = value;
            }

            db::resultset_type db::execute(const string &sql)
            {
                if (db_ == nullptr) {
                    throw database_exception("database is not open");
                }

                PGresult *res = PQexec(db_.get(), sql.c_str());

                if (PQresultStatus(res) != PGRES_TUPLES_OK && PQresultStatus(res) != PGRES_COMMAND_OK) {
                    throw database_exception(last_error());
                }

                return resultset_type(make_shared<resultset>(this, shared_ptr<PGresult>(res, helper::res_delete())));
            }

            shared_ptr<db::statement_type> db::create_statement()
            {
                return make_shared<statement>(this);
            }

            string db::insert_sql(const std::shared_ptr<schema> &schema, const vector<string> &columns) const
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
        }
    }
}

#endif

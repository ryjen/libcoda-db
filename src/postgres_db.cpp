#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPQ

#include "postgres_db.h"
#include "postgres_statement.h"
#include "postgres_resultset.h"
#include "select_query.h"

namespace arg3
{
    namespace db
    {
        namespace helper
        {
            void postgres_res_delete::operator()(PGresult *p) const
            {
                if (p != nullptr) {
                    PQclear(p);
                }
            }

            struct postgres_close_db {
                void operator()(PGconn *p) const
                {
                    if (p != nullptr) {
                        PQfinish(p);
                    }
                }
            };
        }

        postgres_db::postgres_db(const uri &info) : sqldb(info), db_(nullptr), lastId_(0), lastNumChanges_(0)
        {
        }

        postgres_db::postgres_db(const postgres_db &other)
            : sqldb(other), db_(other.db_), lastId_(other.lastId_), lastNumChanges_(other.lastNumChanges_)
        {
        }

        postgres_db::postgres_db(postgres_db &&other) : sqldb(other), db_(other.db_), lastId_(other.lastId_), lastNumChanges_(other.lastNumChanges_)
        {
            other.db_ = nullptr;
        }

        postgres_db &postgres_db::operator=(const postgres_db &other)
        {
            sqldb::operator=(other);

            db_ = other.db_;
            lastId_ = other.lastId_;
            lastNumChanges_ = other.lastNumChanges_;

            return *this;
        }

        postgres_db &postgres_db::operator=(postgres_db &&other)
        {
            sqldb::operator=(std::move(other));

            db_ = other.db_;
            lastId_ = other.lastId_;
            lastNumChanges_ = other.lastNumChanges_;
            other.db_ = nullptr;

            return *this;
        }

        postgres_db::~postgres_db()
        {
        }

        void postgres_db::open()
        {
            if (db_ != nullptr) return;

            PGconn *conn = PQconnectdb(connection_info().value.c_str());

            if (PQstatus(conn) != CONNECTION_OK) {
                throw database_exception(PQerrorMessage(conn));
            }

            db_ = shared_ptr<PGconn>(conn, helper::postgres_close_db());
        }

        bool postgres_db::is_open() const
        {
            return db_ != nullptr;
        }

        void postgres_db::close()
        {
            if (db_ != nullptr) {
                db_ = nullptr;
            }
        }

        string postgres_db::last_error() const
        {
            if (db_ == nullptr) {
                return string();
            }

            return PQerrorMessage(db_.get());
        }

        long long postgres_db::last_insert_id() const
        {
            // TODO: might have to perform a query here
            return lastId_;
        }

        void postgres_db::set_last_insert_id(long long value)
        {
            lastId_ = value;
        }

        int postgres_db::last_number_of_changes() const
        {
            // TODO: might have to perform a query here
            return lastNumChanges_;
        }

        void postgres_db::set_last_number_of_changes(int value)
        {
            lastNumChanges_ = value;
        }

        resultset postgres_db::execute(const string &sql)
        {
            if (db_ == nullptr) {
                throw database_exception("database is not open");
            }

            PGresult *res = PQexec(db_.get(), sql.c_str());

            if (PQresultStatus(res) != PGRES_TUPLES_OK && PQresultStatus(res) != PGRES_COMMAND_OK) {
                throw database_exception(last_error());
            }

            return resultset(make_shared<postgres_resultset>(this, shared_ptr<PGresult>(res, helper::postgres_res_delete())));
        }

        shared_ptr<statement> postgres_db::create_statement()
        {
            return make_shared<postgres_statement>(this);
        }
    }
}

#endif

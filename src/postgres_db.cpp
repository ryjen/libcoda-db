#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBPOSTGRES

#include "postgres_db.h"
#include "postgres_statement.h"
#include "postgres_resultset.h"

namespace arg3
{
    namespace db
    {
        postgres_db::postgres_db(const uri &info) : sqldb(info), db_(nullptr), schema_factory_(this)
        {
        }

        postgres_db::postgres_db(const postgres_db &other) : sqldb(other), db_(nullptr), schema_factory_(other.schema_factory_)
        {
        }

        postgres_db::postgres_db(postgres_db &&other) : sqldb(other), db_(other.db_), schema_factory_(std::move(other.schema_factory_))
        {
            other.db_ = nullptr;
        }

        postgres_db &postgres_db::operator=(const postgres_db &other)
        {
            db_ = nullptr;
            schema_factory_ = other.schema_factory_;

            return *this;
        }

        postgres_db &postgres_db::operator=(postgres_db &&other)
        {
            db_ = other.db_;
            schema_factory_ = std::move(other.schema_factory_);
            other.db_ = nullptr;

            return *this;
        }

        postgres_db::~postgres_db()
        {
            close();
        }

        schema_factory *postgres_db::schemas()
        {
            return &schema_factory_;
        }

        void postgres_db::set_connection_info(const string &value)
        {
            conn_info_.parse(value);

            if (conn_info_.protocol != "postgres" && conn_info_.protocol != "postgresql") {
                throw database_exception("connection protocol is not postgres");
            }
        }

        void postgres_db::query_schema(const string &tableName, std::vector<column_definition> &columns)
        {
            if (!is_open()) return;

            select_query pkq(db_, "information_schema.table_constraints tc", {"tc.table_schema, tc.table_name, kc.column_name"});

            pkq.join(join("information_schema.key_column kc").on("kc.table_name", "tc.table_name").and ("kc.table_schema", "tc.table_schema"));

            pkq.where(where("tc_constraint_type = 'PRIMARY_KEY'") && "kc.position_in_unique_constraint is not null");

            pkq.order_by("tc.table_schema, tc.table_name, kc.position_in_unique_constraint");

            auto primary_keys = execute(pkq);

            select_query info_schema("information_schema.columns", {"column_name", "data_type"});

            info_schema.where("table_name = " + tableName);

            auto rs = execute(info_schema);

            for (auto &row : rs) {
                column_definition def;

                // column name
                def.name = row["column_name"].to_value().to_string();

                if (def.name.empty()) {
                    continue;
                }

                for (auto &pk : primary_keys) {
                    if (pk["column_name"] == def.name) {
                        def.pk = true;
                    }
                }

                // find type
                def.type = row["data_type"].to_value();

                columns.push_back(def);
            }
        }

        void postgres_db::open()
        {
            if (db_ != nullptr) return;

            db_ = postgres_init(nullptr);

            if (postgres_real_connect(db_, host_.c_str(), user_.c_str(), password_.c_str(), dbName_.c_str(), port_, nullptr, 0) == nullptr) {
                throw database_exception("No connection could be made to the database");
            }
        }

        bool postgres_db::is_open() const
        {
            return db_ != nullptr;
        }

        void postgres_db::close()
        {
            if (db_ != nullptr) {
                postgres_close(db_);
                db_ = nullptr;
            }
        }

        string postgres_db::last_error() const
        {
            if (db_ == nullptr) {
                return string();
            }

            ostringstream buf;

            buf << postgres_errno(db_);
            buf << ": " << postgres_error(db_);

            return buf.str();
        }

        long long postgres_db::last_insert_id() const
        {
            if (db_ == nullptr) {
                return 0;
            }

            return postgres_insert_id(db_);
        }

        int postgres_db::last_number_of_changes() const
        {
            if (db_ == nullptr) {
                return 0;
            }

            return postgres_affected_rows(db_);
        }

        resultset postgres_db::execute(const string &sql, bool cache)
        {
            if (db_ == nullptr) {
                throw database_exception("database is not open");
            }

            MYSQL_RES *res;

            if (postgres_real_query(db_, sql.c_str(), sql.length())) throw database_exception(last_error());

            res = postgres_store_result(db_);

            if (res == nullptr && postgres_field_count(db_) != 0) {
                throw database_exception(last_error());
            }

            if (cache)
                return resultset(make_shared<postgres_cached_resultset>(this, shared_ptr<MYSQL_RES>(res, postgres_res_delete())));
            else
                return resultset(make_shared<postgres_resultset>(this, shared_ptr<MYSQL_RES>(res, postgres_res_delete())));
        }

        shared_ptr<statement> postgres_db::create_statement()
        {
            return make_shared<postgres_statement>(this);
        }
    }
}

#endif

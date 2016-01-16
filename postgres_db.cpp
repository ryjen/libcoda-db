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
        
    	postgres_db::postgres_db(const string &connInfo)
            : sqldb(), db_(nullptr), conn_info_(), schema_factory_(this)
        {
		set_connection_info(connInfo);
        }

        postgres_db::postgres_db(const postgres_db &other)
            : sqldb(other),
              db_(nullptr),
	      conn_info_(other.conn_info_),
              schema_factory_(other.schema_factory_)
        {
        }

        postgres_db::postgres_db(postgres_db &&other)
            : sqldb(other),
              db_(other.db_),
              conn_info_(std::move(other.conn_info_)),
	      schema_factory_(std::move(other.schema_factory_))
        {
            other.db_ = nullptr;
        }

        postgres_db &postgres_db::operator=(const postgres_db &other)
        {
            db_ = nullptr;
            conn_info_ = other.conn_info_;
	    schema_factory_ = other.schema_factory_;

            return *this;
        }

        postgres_db &postgres_db::operator=(postgres_db &&other)
        {
            db_ = other.db_;
            conn_info_ = std::move(other.conn_info_);
	    schema_factory_ = std::move(other.schema_factory_);
            other.db_ = nullptr;

            return *this;
        }

        postgres_db::~postgres_db()
        {
            close();
        }

        string postgres_db::connection_info() const
        {
	    return conn_info_.to_string();
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

            auto rs = execute("select column_name as Field, data_type as Type from INFORMATION_SCHEMA.COLUMNS where table_name = '" + tableName + "'");

            for (auto &row : rs) {
                column_definition def;

                // column name
                def.name = row["Field"].to_value().to_string();

                if (def.name.empty()) {
                    continue;
                }

		auto pkrs = execute("select tc.table_schema, tc.table_name, kc.column_name
				from 
				    information_schema.table_constraints tc
				        join information_schema.key_column_usage kc 
					        on kc.table_name = tc.table_name and kc.table_schema = tc.table_schema
						where 
						    tc.constraint_type = 'PRIMARY KEY'
						        and kc.position_in_unique_constraint is not null
							order by tc.table_schema,
							         tc.table_name,
								          kc.position_in_unique_constraint;")
                // primary key check
                def.pk = row["Key"].to_value() == "PRI";

                // find type
                string type = row["Type"].to_value();


                // yes, this is pretty immature
                if (type.find("int") != string::npos) {
                    def.type = MYSQL_TYPE_LONG;
                } else if (type.find("double") != string::npos) {
                    def.type = MYSQL_TYPE_DOUBLE;
                } else if (type.find("blob") != string::npos) {
                    def.type = MYSQL_TYPE_BLOB;
                } else {
                    def.type = MYSQL_TYPE_STRING;
                }

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
            if(db_ == nullptr) {
                return string();
            }

            ostringstream buf;

            buf << postgres_errno(db_);
            buf << ": " << postgres_error(db_);

            return buf.str();
        }

        long long postgres_db::last_insert_id() const
        {
            if(db_ == nullptr) {
                return 0;
            }

            return postgres_insert_id(db_);
        }

        int postgres_db::last_number_of_changes() const
        {
            if(db_ == nullptr) {
                return 0;
            }

            return postgres_affected_rows(db_);
        }

        resultset postgres_db::execute(const string &sql, bool cache)
        {
            if(db_ == nullptr) {
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


/*!
 * @copyright ryan jennings (arg3.com), 2013
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <algorithm>
#include "sqldb.h"
#include "query.h"
#include "exception.h"
#include "resultset.h"
#include "sqlite/db.h"
#include "mysql/db.h"
#include "postgres/db.h"
#include "select_query.h"

namespace arg3
{
    namespace db
    {
        ARG3_IMPLEMENT_EXCEPTION(database_exception, std::exception);

        ARG3_IMPLEMENT_EXCEPTION(no_such_column_exception, database_exception);

        ARG3_IMPLEMENT_EXCEPTION(record_not_found_exception, database_exception);

        ARG3_IMPLEMENT_EXCEPTION(binding_error, database_exception);

        void uri::parse(const string &url_s)
        {
            value = url_s;
            // do the manual implementation from stack overflow
            // with some mods for the port
            const string prot_end("://");
            string::const_iterator prot_i = search(url_s.begin(), url_s.end(), prot_end.begin(), prot_end.end());
            protocol.reserve(distance(url_s.begin(), prot_i));
            transform(url_s.begin(), prot_i, back_inserter(protocol), ptr_fun<int, int>(tolower));  // protocol is icase
            if (prot_i == url_s.end()) {
                return;
            }

            advance(prot_i, prot_end.length());

            string::const_iterator user_i = find(prot_i, url_s.end(), '@');
            string::const_iterator path_i;

            if (user_i != url_s.end()) {
                string::const_iterator pwd_i = find(prot_i, user_i, ':');

                if (pwd_i != user_i) {
                    password.assign(pwd_i, user_i);
                    user.assign(prot_i, pwd_i);
                } else {
                    user.assign(prot_i, user_i);
                }
                path_i = user_i;
            } else {
                path_i = find(prot_i, url_s.end(), '/');
                if (path_i == url_s.end()) {
                    path_i = prot_i;
                }
            }
            string::const_iterator port_i = find(prot_i, path_i, ':');
            string::const_iterator host_end;
            if (port_i != url_s.end()) {
                port.assign(port_i, path_i);
                host_end = port_i;
            } else {
                host_end = path_i;
            }
            host.reserve(distance(prot_i, host_end));
            transform(prot_i, host_end, back_inserter(host), ptr_fun<int, int>(tolower));  // host is icase
            string::const_iterator query_i = find(path_i, url_s.end(), '?');
            path.assign(path_i, query_i);
            if (query_i != url_s.end()) ++query_i;
            query.assign(query_i, url_s.end());
        }

        shared_ptr<sqldb> sqldb::from_uri(const string &uristr)
        {
            db::uri uri(uristr);
#ifdef HAVE_LIBSQLITE3
            if ("file" == uri.protocol) return make_shared<sqlite3_db>(uri);
#endif
#ifdef HAVE_LIBMYSQLCLIENT
            if ("mysql" == uri.protocol) return make_shared<mysql::db>(uri);
#endif
#ifdef HAVE_LIBPQ
            if ("postgres" == uri.protocol || "postgresql" == uri.protocol) return make_shared<postgres_db>(uri);
#endif
            throw database_exception("unknown database " + uri.value);
        }

        sqldb::sqldb() : schema_factory_(this)
        {
        }

        sqldb::sqldb(const uri &connectionInfo) : connectionInfo_(connectionInfo), schema_factory_(this)
        {
        }

        void sqldb::query_schema(const string &tableName, std::vector<column_definition> &columns)
        {
            if (!is_open()) return;

            select_query pkq(this, "information_schema.table_constraints tc", {"tc.table_schema, tc.table_name, kc.column_name"});

            pkq.join("information_schema.key_column_usage kc").on("kc.table_name = tc.table_name") && "kc.table_schema = tc.table_schema";

            pkq.where("tc.constraint_type = 'PRIMARY KEY'") && "tc.table_name = $1";

            pkq.order_by("tc.table_schema, tc.table_name, kc.position_in_unique_constraint");

            pkq.bind(1, tableName);

            auto primary_keys = pkq.execute();

            select_query info_schema(this, "information_schema.columns", {"column_name", "data_type"});

            info_schema.where("table_name = $1");

            info_schema.bind(1, tableName);

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
                }

                // find type
                def.type = row["data_type"].to_value().to_string();

                columns.push_back(def);
            }
        }

        uri sqldb::connection_info() const
        {
            return connectionInfo_;
        }

        void sqldb::set_connection_info(const uri &value)
        {
            connectionInfo_ = value;
        }


        schema_factory *sqldb::schemas()
        {
            return &schema_factory_;
        }
    }
}

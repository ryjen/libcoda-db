/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <algorithm>
#include "sqldb.h"
#include "query.h"
#include "exception.h"
#include "resultset.h"
#include "sqlite3_db.h"
#include "mysql_db.h"
#include "postgres_db.h"

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
            if ("mysql" == uri.protocol) return make_shared<mysql_db>(uri);
#endif
#ifdef HAVE_LIBPQ
            if ("postgres" == uri.protocol || "postgresql" == uri.protocol) return make_shared<postgres_db>(uri);
#endif
            throw database_exception("unknown database " + uri.value);
        }

        sqldb::sqldb(const uri &connectionInfo) : connectionInfo_(connectionInfo), cacheLevel_(CACHE_NONE), schema_factory_(this)
        {
        }

        resultset sqldb::execute(const string &sql)
        {
            return execute(sql, cache_level() == sqldb::CACHE_RESULTSET);
        }

        void sqldb::set_cache_level(CacheLevel level)
        {
            cacheLevel_ = level;
        }

        sqldb::CacheLevel sqldb::cache_level() const
        {
            return cacheLevel_;
        }

        uri sqldb::connection_info() const
        {
            return connectionInfo_;
        }


        schema_factory *sqldb::schemas()
        {
            return &schema_factory_;
        }
    }
}

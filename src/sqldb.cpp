/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "sqldb.h"
#include "base_query.h"
#include "exception.h"
#include "resultset.h"
#include "sqlite3_db.h"
#include "mysql_db.h"

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

            const string prot_end("://");
            string::const_iterator prot_i = search(url_s.begin(), url_s.end(),
                                                   prot_end.begin(), prot_end.end());
            protocol.reserve(distance(url_s.begin(), prot_i));
            transform(url_s.begin(), prot_i,
                      back_inserter(protocol),
                      ptr_fun<int, int>(tolower)); // protocol is icase
            if ( prot_i == url_s.end() )
                return;
            advance(prot_i, prot_end.length());

            string::const_iterator user_i = find(prot_i, url_s.end(), '@');
            string::const_iterator path_i;

            if (user_i != url_s.end())
            {
                string::const_iterator pwd_i = find(prot_i, user_i, ':');

                if (pwd_i != user_i)
                {
                    password.assign(pwd_i, user_i);
                    user.assign(prot_i, pwd_i);
                }
                else
                {
                    user.assign(prot_i, user_i);
                }
                path_i = user_i;
            }
            else path_i = find(prot_i, url_s.end(), '/');
            host.reserve(distance(prot_i, path_i));
            transform(prot_i, path_i,
                      back_inserter(host),
                      ptr_fun<int, int>(tolower)); // host is icase
            string::const_iterator query_i = find(path_i, url_s.end(), '?');
            path.assign(path_i, query_i);
            if ( query_i != url_s.end() )
                ++query_i;
            query.assign(query_i, url_s.end());
        }

        shared_ptr<sqldb> get_db_from_uri(const string &uristr)
        {
            db::uri uri(uristr);

            if ("file" == uri.protocol)
                return make_shared<sqlite3_db>(uri);
            else if ("mysql" == uri.protocol)
                return make_shared<mysql_db>(uri);
            else
                throw database_exception("unknown database " + uri.value);
        }

        sqldb::sqldb() : logLevel_(NONE)
        {}

        void sqldb::set_log_level(LogLevel level)
        {
            logLevel_ = level;
        }

        void sqldb::log(LogLevel level, const string &message)
        {
            if (logLevel_ >= level)
                cout << "sqldb: " << message << endl;
        }
    }
}

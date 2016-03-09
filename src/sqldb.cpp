#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sqldb.h"
#include "exception.h"
#include "sqlite/session.h"
#include "mysql/session.h"
#include "postgres/session.h"

namespace arg3
{
    namespace db
    {
        ARG3_IMPLEMENT_EXCEPTION(database_exception, std::exception);

        ARG3_IMPLEMENT_EXCEPTION(no_such_column_exception, database_exception);

        ARG3_IMPLEMENT_EXCEPTION(record_not_found_exception, database_exception);

        ARG3_IMPLEMENT_EXCEPTION(binding_error, database_exception);

        ARG3_IMPLEMENT_EXCEPTION(transaction_exception, database_exception);

        ARG3_IMPLEMENT_EXCEPTION(no_primary_key_exception, database_exception);

        session sqldb::create_session(const std::string &uristr)
        {
            db::uri uri(uristr);
            return create_session(uri);
        }
        session sqldb::create_session(const uri &uri)
        {
            auto factory = instance()->factories_[uri.protocol];

            if (factory == nullptr) {
                throw database_exception("unknown database " + uri.value);
            }

            return session(std::shared_ptr<arg3::db::session_impl>(factory->create(uri)));
        }

        void sqldb::register_session(const std::string &protocol, const std::shared_ptr<session_factory> &factory)
        {
            if (protocol.empty()) {
                throw database_exception("invalid protocol for session factory registration");
            }

            if (factory == nullptr) {
                throw database_exception("invalid factory for session factory registration");
            }

            instance()->factories_[protocol] = factory;
        }

        sqldb *sqldb::instance()
        {
            static sqldb instance_;
            return &instance_;
        }

        sqldb::sqldb()
        {
#ifdef HAVE_LIBSQLITE3
            auto sqlite_factory = std::make_shared<sqlite::factory>();
            factories_["sqlite"] = sqlite_factory;
            factories_["file"] = sqlite_factory;
#endif
#ifdef HAVE_LIBMYSQLCLIENT
            auto mysql_factory = std::make_shared<mysql::factory>();
            factories_["mysql"] = mysql_factory;
#endif
#ifdef HAVE_LIBPQ
            auto postgres_factory = std::make_shared<postgres::factory>();
            factories_["postgres"] = postgres_factory;
            factories_["postgresql"] = postgres_factory;
#endif
        }

        sqldb::~sqldb()
        {
        }
    }
}
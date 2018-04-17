
#include "sqldb.h"
#include "exception.h"
#include "session.h"
#include "session_factory.h"

namespace coda {
    namespace db {
        std::shared_ptr<session> sqldb::create_session(const std::string &uristr) {
            db::uri uri(uristr);
            return create_session(uri);
        }

        std::shared_ptr<session> sqldb::create_session(const uri &uri) {
            auto factory = instance()->factories_[uri.protocol];

            if (factory == nullptr) {
                throw database_exception("unknown database " + uri.value);
            }

            return std::make_shared<session>(factory->create(uri));
        }

        std::shared_ptr<session> sqldb::open_session(const std::string &uristr) {
            db::uri uri(uristr);
            return open_session(uri);
        }

        std::shared_ptr<session> sqldb::open_session(const uri &uri) {
            auto factory = instance()->factories_[uri.protocol];

            if (factory == nullptr) {
                throw database_exception("unknown database " + uri.value);
            }

            auto value = std::make_shared<session>(factory->create(uri));

            value->open();

            return value;
        }

        void sqldb::register_session(const std::string &protocol, const std::shared_ptr<session_factory> &factory) {
            if (protocol.empty()) {
                throw database_exception("invalid protocol for session factory registration");
            }

            if (factory == nullptr) {
                throw database_exception("invalid factory for session factory registration");
            }

            instance()->factories_[protocol] = factory;
        }

        sqldb *sqldb::instance() {
            static sqldb instance_;
            return &instance_;
        }

        sqldb::sqldb() {
        }

        sqldb::~sqldb() {
        }
    }
}
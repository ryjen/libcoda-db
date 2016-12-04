#ifndef RJ_TEST_DB_H
#define RJ_TEST_DB_H

#include <unistd.h>
#include <list>
#include "mysql/session.h"
#include "postgres/session.h"
#include "record.h"
#include "sqldb.h"
#include "sqlite/session.h"
#include "uri.h"

namespace rj
{
    namespace db
    {
        namespace test
        {
            namespace spec
            {
                typedef std::function<void()> type;

                void load();
            };

            class session
            {
               public:
                virtual void setup() = 0;
                virtual void teardown() = 0;
            };

            class factory : public rj::db::session_factory
            {
               public:
                std::shared_ptr<rj::db::session_impl> create(const rj::db::uri &value);
            };

            extern std::shared_ptr<rj::db::session> current_session;

            extern void register_current_session();

            extern void setup_current_session();

            extern void teardown_current_session();

            extern void unregister_current_session();

            class user : public rj::db::record<user>
            {
               public:
                constexpr static const char *const TABLE_NAME = "users";

                using rj::db::record<user>::record;

                user(const std::shared_ptr<rj::db::session> &sess = current_session) : record(sess->get_schema(TABLE_NAME))
                {
                }

                user(long long id, const std::shared_ptr<rj::db::session> &sess = current_session) : user(sess->get_schema(TABLE_NAME))
                {
                    set_id(id);
                    refresh();
                }

                /*!
                 * required constructor
                 */
                user(const std::shared_ptr<rj::db::schema> &schema) : record(schema)
                {
                }

                std::string to_string()
                {
                    std::ostringstream buf;

                    buf << id() << ": " << get("first_name") << " " << get("last_name");

                    return buf.str();
                }
            };
        }
    }
}

#define specification(name, fn) rj::db::test::spec::type spec_file_##name(fn)

#endif

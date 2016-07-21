#ifndef RJ_TEST_DB_H
#define RJ_TEST_DB_H

#include <unistd.h>
#include "mysql/session.h"
#include "postgres/session.h"
#include "record.h"
#include "sqldb.h"
#include "sqlite/session.h"
#include "uri.h"

#if !defined(HAVE_LIBMYSQLCLIENT) && !defined(HAVE_LIBSQLITE3) && !defined(HAVE_LIBPQ)
#error "Mysql, postgres or sqlite is not installed on the system"
#endif

class test_session
{
   public:
    virtual void setup() = 0;
    virtual void teardown() = 0;
};

#if defined(HAVE_LIBSQLITE3)

class test_sqlite3_factory : public rj::db::session_factory
{
   public:
    std::shared_ptr<rj::db::session_impl> create(const rj::db::uri &value);
};

class test_sqlite3_session : public rj::db::sqlite::session, public test_session
{
    friend class test_sqlite3_factory;

   public:
    using rj::db::sqlite::session::session;

    void setup();

    void teardown();
};

#endif

#if defined(HAVE_LIBMYSQLCLIENT)

class test_mysql_factory : public rj::db::session_factory
{
   public:
    std::shared_ptr<rj::db::session_impl> create(const rj::db::uri &value);
};

class test_mysql_session : public rj::db::mysql::session, public test_session
{
    friend class test_mysql_factory;

   public:
    using rj::db::mysql::session::session;

    void setup();

    void teardown();
};

#endif

#if defined(HAVE_LIBPQ)

class test_postgres_factory : public rj::db::session_factory
{
   public:
    std::shared_ptr<rj::db::session_impl> create(const rj::db::uri &value);
};

class test_postgres_session : public rj::db::postgres::session, public test_session
{
    friend class test_postgres_factory;

   public:
    using rj::db::postgres::session::session;

    void setup();

    void teardown();
};

#endif

extern std::shared_ptr<rj::db::session> current_session;

void register_test_sessions();

void setup_current_session();

void teardown_current_session();

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


#endif

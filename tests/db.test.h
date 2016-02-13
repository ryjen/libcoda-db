#ifndef ARG3_TEST_DB_H
#define ARG3_TEST_DB_H

#include "sqldb.h"
#include "record.h"
#include <unistd.h>
#include "sqlite/db.h"
#include "mysql/db.h"
#include "postgres/db.h"

#if !defined(HAVE_LIBMYSQLCLIENT) && !defined(HAVE_LIBSQLITE3) && !defined(HAVE_LIBPQ)
#error "Mysql, postgres or sqlite is not installed on the system"
#endif

class test_db
{
   public:
    virtual void setup() = 0;
    virtual void teardown() = 0;
};

#if defined(HAVE_LIBSQLITE3) && defined(TEST_SQLITE)

class test_sqlite3_db : public arg3::db::sqlite::db, public test_db
{
   public:
    test_sqlite3_db() : db(arg3::db::uri("file://testdb.db"))
    {
    }

    void setup();

    void teardown();

    sqlite3 *rawDb()
    {
        return db_.get();
    }
};

extern test_sqlite3_db sqlite_testdb;
#endif

#if defined(HAVE_LIBMYSQLCLIENT) && defined(TEST_MYSQL)

class test_mysql_db : public arg3::db::mysql::db, public test_db
{
   public:
    test_mysql_db() : db(arg3::db::uri("mysql://test"))
    {
    }

    void setup();

    void teardown();

    MYSQL *rawDb()
    {
        return db_.get();
    }
};
extern test_mysql_db mysql_testdb;

#endif

#if defined(HAVE_LIBPQ) && defined(TEST_POSTGRES)

class test_postgres_db : public arg3::db::postgres::db, public test_db
{
   public:
    test_postgres_db() : db(arg3::db::uri("postgres://localhost/test"))
    {
    }

    void setup();

    void teardown();

    PGconn *rawDb()
    {
        return db_.get();
    }
};

extern test_postgres_db postgres_testdb;

#endif

extern arg3::db::sqldb *testdb;

void setup_testdb();

void teardown_testdb();

class user : public arg3::db::record<user>
{
   public:
    user(arg3::db::sqldb *db = testdb) : record(db, "users", "id")
    {
    }

    user(const arg3::db::row &values, arg3::db::sqldb *db = testdb) : record(db, "users", "id", values)
    {
    }

    user(long long id, arg3::db::sqldb *db = testdb) : record(db, "users", "id", id)
    {
    }

    user(const user &other) : record(other)
    {
    }

    user(user &&other) : record(std::move(other))
    {
    }

    ~user()
    {
    }

    user &operator=(const user &other)
    {
        record<user>::operator=(other);
        return *this;
    }

    user &operator=(user &&other)
    {
        record<user>::operator=(std::move(other));
        return *this;
    }
    std::string to_string()
    {
        std::ostringstream buf;

        buf << id() << ": " << get("first_name") << " " << get("last_name");

        return buf.str();
    }
};


#endif

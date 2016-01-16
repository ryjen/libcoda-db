#ifndef ARG3_TEST_DB_H
#define ARG3_TEST_DB_H

#include "sqldb.h"
#include "base_record.h"
#include <unistd.h>
#include "sqlite3_db.h"
#include "mysql_db.h"

#ifdef HAVE_LIBSQLITE3

class test_sqlite3_db : public arg3::db::sqlite3_db
{
   public:
    test_sqlite3_db() : sqlite3_db(arg3::db::uri("file://testdb.db"))
    {
    }

    void setup();

    void teardown();

    sqlite3 *rawDb()
    {
        return db_;
    }
};

extern test_sqlite3_db sqlite_testdb;
#endif

#ifdef HAVE_LIBMYSQLCLIENT

class test_mysql_db : public arg3::db::mysql_db
{
   public:
    test_mysql_db() : mysql_db(arg3::db::uri("mysql://test"))
    {
    }

    void setup();

    void teardown();

    MYSQL *rawDb()
    {
        return db_;
    }
};
extern test_mysql_db mysql_testdb;

#endif

extern void setup_testdb();
extern void teardown_testdb();

extern arg3::db::sqldb *testdb;

class user : public arg3::db::base_record<user>
{
   public:
    user(arg3::db::sqldb *db = testdb) : base_record(db, "users", "id")
    {
    }

    user(const arg3::db::row &values, arg3::db::sqldb *db = testdb) : base_record(db, "users", "id", values)
    {
    }

    user(long long id, arg3::db::sqldb *db = testdb) : base_record(db, "users", "id", id)
    {
    }

    user(const user &other) : base_record(other)
    {
    }

    user(user &&other) : base_record(std::move(other))
    {
    }

    ~user()
    {
    }

    user &operator=(const user &other)
    {
        base_record<user>::operator=(other);
        return *this;
    }

    user &operator=(user &&other)
    {
        base_record<user>::operator=(std::move(other));
        return *this;
    }
    string to_string()
    {
        ostringstream buf;

        buf << id() << ": " << get("first_name") << " " << get("last_name");

        return buf.str();
    }
};


#endif

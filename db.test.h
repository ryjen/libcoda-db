#ifndef ARG3_TEST_DB_H
#define ARG3_TEST_DB_H

#include "sqldb.h"
#include "base_record.h"
#include <unistd.h>

#define TESTDB "test.db"

class testsqldb : public arg3::db::sqlite3_db
{
public:
    testsqldb() : sqlite3_db(TESTDB) {}

    void setup()
    {
        open();
        execute("create table if not exists users(id integer primary key autoincrement, first_name varchar(45), last_name varchar(45))");
    }

    void teardown()
    {
        close();
        unlink(connection_string().c_str());
        schemas()->clear("users");
    }

    sqlite3 *rawDb() const
    {
        return db_;
    }
};

extern testsqldb testdb;

class user : public arg3::db::base_record<user>
{
public:
    user() : base_record(&testdb, "users", "id") {}

    user(const arg3::db::row &values) : base_record(&testdb, "users", "id", values) {}

    user(long long id) : base_record(&testdb, "users", "id", id) {}

    user(const user &other) : base_record(other) {}

    user(user &&other) : base_record(other) {}

    user &operator=(const user &other)
    {
        base_record<user>::operator=(other);
        return *this;
    }

    user &operator=(user && other)
    {
        base_record<user>::operator=(other);
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

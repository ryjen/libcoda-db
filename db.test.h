#ifndef ARG3_TEST_DB_H
#define ARG3_TEST_DB_H

#include "sqldb.h"
#include "base_record.h"
#include <unistd.h>

#define TESTDB "test.db"

class testsqldb : public arg3::db::sqldb
{
public:
    testsqldb() : sqldb(TESTDB) {}

    void setup()
    {
        open();
        execute("create table if not exists users(id integer primary key autoincrement, first_name varchar(45), last_name varchar(45))");
    }

    void teardown()
    {
        close();
        unlink(filename().c_str());
        arg3::db::record_schema::clear("users");
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
    user() : base_record(&testdb, "users") {}

    user(const arg3::db::row &values) : base_record(&testdb, "users", values) {}

    user(long long id) : base_record(&testdb, "users", id) {}

    string to_string()
    {
        ostringstream buf;

        buf << id() << ": " << get("first_name") << " " << get("last_name");

        return buf.str();
    }
};


#endif

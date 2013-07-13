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
    }

    sqlite3 *rawDb() const {
        return db_;
    }
};

extern testsqldb testdb;

class user : public arg3::db::base_record<user>
{
public:
    user() {}

    user(const arg3::db::row &values) : base_record(values) {}

    arg3::db::sqldb* db() const
    {
        return &testdb;
    }

    string tableName() const {
        return "users";
    }

    arg3::db::sql_value id() const {

        return get("id");
    }

    string to_string()
    {
        ostringstream buf;

        buf << get("id") << ": " << get("first_name") << " " << get("last_name");

        return buf.str();
    }
};


#endif

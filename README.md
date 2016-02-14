
libarg3db
=========

[![Build Status](http://img.shields.io/travis/ryjen/arg3db.svg)](https://travis-ci.org/ryjen/arg3db)
[![Coverage Status](https://coveralls.io/repos/ryjen/arg3db/badge.svg?branch=master&service=github)](https://coveralls.io/github/ryjen/arg3db?branch=master)
[![License](http://img.shields.io/:license-mit-blue.svg)](http://ryjen.mit-license.org)

a sqlite, mysql and postgres wrapper / active record (ish) implementation

Use in production at your own risk, still doing quite a bit of testing, refactoring and new features.  Pull requests are welcomed...

Why
---

Purely selfish reasons.

Building
--------

After cloning run the following command to initialize submodules:

```bash

git submodule update --init --recursive
```

use [cmake](https://cmake.org) to generate for the build system of your choice.

```bash
mkdir debug; cd debug;
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
make test
```

options supported are:

    -DCODE_COVERAGE=ON   :   enable code coverage using lcov
    -DMEMORY_CHECK=ON    :   enable valgrind memory checking on tests

Model
-----

      /* database interfaces */
      sqldb                                 - interface for a specific database
        └ statement                         - interface for a prepared statement
              └ resultset                   - results of a statement
                    └ row                   - a single result
                         └ column           - a field in a row containing a value

      /* implementations using the above*/
      schema                                - a definition of a table
      schema_factory                        - cached schemas
      base_record                           - the active record (ish) implementation
      select_query                          - builds select queries
      insert_query                          - inserts data
      update_query                          - updates data
      delete_query                          - builds delete queries
      sql_value                             - storage and conversion for basic sql types


Records
=======

An simple user example
----------------------
```c++
arg3::db::sqlite::db testdb("test.db");

/* Other databases

arg3::db::mysql::db testdb(arg3::db::uri("mysql://user@pass:localhost:3306/database"));
arg3::db::postgres::db testdb(arg3::db::uri("postgres://localhost/test"))

*/

class user : public arg3::db::record<user>
{
    constexpr static const char *const ID_COLUMN = "id";
    constexpr static const char *const TABLE_NAME = "users";
public:
    /* default constructor, no database hits */
    user() : record(&testdb, TABLE_NAME, ID_COLUMN) {}

    /* results constructor */
    user(const row &values) : record(&testdb, TABLE_NAME, ID_COLUMN, values) {}

    /* id constructor, pulls data from database */
    user(long id) : record(&testdb, TABLE_NAME, ID_COLUMN, id) {}

    /* utility method showing how to get columns */
    string to_string() const
    {
        ostringstream buf;

        buf << id() << ": " << get("first_name") << " " << get("last_name");

        return buf.str();
    }

    // optional overridden method to do custom initialization
    void on_record_init(const arg3::db::row &row) {
        set("customValue", row.column("customName").to_value());
    }   

    // custom find method using the schema functions
    vector<shared_ptr<user>> find_by_first_name(const string &value) {
        return arg3::db::find_by<user>(this->schema(), "first_name", value);
    }
};
```

Query records
-------------
```c++
    user obj;

    /* find all users with a callback */
    obj.find_all([](const shared_ptr<user> &record) {
        cout << "User: " << record->to_string() << endl;
    }

    /* find specific records and return an result vector */
    results = obj.find_by("first_name", "Joe");

    for (auto user : results) {
        cout << "Found user: " << user->to_string() << endl;
    }

    /* can also use schema functions */
    results = find_all<user>(testdb.schemas()->get("users"));

````

Save a record
-------------
```c++
    /* save a user */
    user obj;

    obj.set("first_name", "John");
    obj.set("last_name", "Doe");

    if(!obj.save()) {
    	cerr << testdb.last_error() << endl;
    }
```

Delete a record
---------------
```c++
    user obj(1); // id constructor

    if(!obj.de1ete()) {
        cerr << testdb.last_error() << endl;
    }
```


Prepared Statements
===================

Binding parameters in queries should follow a doller sign index format:

 '$1', '$2', '$3', etc.

 Note that mysql does not support re-using (more than one instance of) parameters in a query string (02/09/16).

 Also not supported yet are named parameters (02/09/16)

Basic Queries
=============

Modify Queries
--------------
```c++
/* insert a  user (INSERT INTO ...) */
insert_query query(&testdb, "users", {"id", "first_name", "last_name"});

/* bind the values to the columns.  TODO: named parameter binding */
query.bind_all(4321, "dave", "patterson");

if (!query.execute())
    cerr << testdb.last_error() << endl;
else
    cout << "last insert id " << query.last_insert_id() << endl;
```

```c++
/* update a user (UPDATE ...) */
update_query query(&testdb, "users", {"id", "first_name", "last_name"});

/* using where clause WHERE .. OR .. */
query.where("id = $4") or ("last_name = $5");

/* bind update columns and where clause */
query.bind_all(1, 3432, "mark", "anthony", 1234, "henry");

query.execute();
```

```c++
/* delete a user (DELETE FROM ...) */
delete_query query(&testdb, "users");

query.where("id = $1 AND first_name = $2", 1234, "bob");

query.execute();

```

Select Query
------------
```c++
/* select some users */
select_query query(&testdb, "users");

query.where("last_name = $1 OR first_name = $2", "Jenkins", "Harry");

auto results = query.execute();

for ( auto &row : results) {
    string lName = row["last_name"]; // "Jenkins"
    // do more stuff
}


```

The select query also supports a call back interface:

```
select_query query(testdb, "users");

query.execute([](const resultset & rs)
{
    // do something with a resultset

    rs.for_each([](const row & r)
    {
        // do something with a row

        r.for_each([](const column & c)
        {
            // do something with a column
        });
    });
});

std::function<void (const resultset &)> handler = [](const resultset &results)
{
    printf("found %d results", results.size());
}

query.execute(handler);
```

Joins
-----

There is a helper class **join_clause** to build join statements, but you can also build them with sql.

```c++

select_query query(&testdb, "users u", {"u.id", "us.setting"});

query.join("user_settings s").on("u.id = s.user_id") and ("s.valid = 1");

query.execute();

```

Batch Queries
-------------
```c++
/* execute some raw sql */
insert_query query(&testdb, "counts");

/* turn on batch mode for this query */
query.set_flags(modify_query::Batch);

for(int i = 1000; i < 3000; i++) {
    query.bind(1, i);

    if (!query.execute()) {
        cerr << testdb.last_error() << endl;
    }
}
```

Types
-----

A **sql_time** class was added for dealing with sql date/time formats.

```c++
time_t current_time = time(0);

/* 
 * create a DATE sql value 
 *
 * can be DATE, TIME, TIMESTAMP
 */
sql_time value(current_time, sql_time::DATE);

/* binds the date to a query */
query.bind(1, value);

/* YYYY-MM-DD format */
auto str = value.to_string();
```

Queries can also handle binary data with **sql_blob**.

```c++
size_t sz = 30;
void *data = malloc(sz);
/* set data here */

/*
 * create a blob value, this will create a copy of the data
 *
 * you can pass function pointers to control how the data is allocated, freed, copied and compared
 */
sql_blob value(data, sz);

query.bind(1, value);
```

Caching
-------

For sqlite3 databases results from a query will have a dependency on a database pointer that must remain open.
Memory caching was add to pre-fetch the values and eliminate the dependency if needed.  It can be done at the resultset, row or column level.

Caching is also used for looking up schemas to reduce hits to the database.

Alternatives
============

- [Poco Data](http://pocoproject.org/docs/00200-DataUserManual.html)
- [SQLAPI++](http://www.sqlapi.com)
- [SOCI](http://soci.sourceforge.net)

TODO / ROADMAP
==============

* More and better quality tests, I demand 100% coverage
* Support named parameter binding

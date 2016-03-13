
libarg3db
=========

[![Build Status](http://img.shields.io/travis/ryjen/arg3db.svg)](https://travis-ci.org/ryjen/arg3db)
[![Coverage Status](https://coveralls.io/repos/ryjen/arg3db/badge.svg?branch=master&service=github)](https://coveralls.io/github/ryjen/arg3db?branch=master)
[![License](http://img.shields.io/:license-mit-blue.svg)](http://ryjen.mit-license.org)

a sqlite, mysql and postgres wrapper / active record (ish) implementation

While this library I think is pretty awesome, use in production at your own risk. Still doing quite a bit of testing, refactoring and new features (2016/02/20).  

Pull requests are welcomed...

Why
---

Purely selfish reasons like learning, resume content, and a lack of desire to use other options.

What makes this library great
-----------------------------

- concept of schema's and automatic primary key usage for records
- support for major databases
- flexible prepared statement parameter usage
- in most cases forced prepared statement usage (no sql injection)
- nice syntax that makes sense (I think)

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
      session                               - interface for an open database session
        └ statement                         - interface for a prepared statement
              └ resultset                   - results of a statement
                    └ row                   - a single result
                         └ column           - a field in a row containing a value

      /* implementations using the above*/
      schema                                - a definition of a table
      schema_factory                        - cached schemas
      record                                - the active record (ish) implementation
      select_query                          - builds select queries
      insert_query                          - inserts data
      update_query                          - updates data
      delete_query                          - builds delete queries
      sql_value                             - storage and conversion for basic sql types
      transaction                           - transactional functionality


Records
=======

An simple user example
----------------------

Records should be implemented using the [curiously reoccuring template pattern (CRTP)](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern).

```c++
auto current_session = sqldb::create_session("file://test.db");

/* Other databases

auto current_session = sqldb::create_session("mysql://user@pass:localhost:3306/database");
auto current_session = sqldb::create_session("postgres://localhost/test");

*/

class user : public arg3::db::record<user>
{
public:
    constexpr static const char *const TABLE_NAME = "users";

    /* only required constructor */
    user(const std::shared_ptr<schema> &schema) : record(schema) 
    {}

    /* default constructor */
    user(const std::shared_ptr<session> &session = current_session) : record(session->get_schema(TABLE_NAME))
    {}

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

Built in record queries include **find_by_id(), find_all(), find_by()** and **find_one()**.  

example using a callback:
```c++
  /* find users with a callback */
  user().find_xxx(... [](const shared_ptr<user> &record) {
      cout << "User: " << record->to_string() << endl;
  });
```
example using a return value:
```c++
  /* find users returning the results */
  auto results = user().find_xxx(...);

  for (auto user : results) {
      cout << "User: " << record->to_string() << endl;
  }
```

the alternative way can be written using schema's instead of the user object:
```c++
  auto schema = current_session->get_schema(user::TABLE_NAME);

  find_xxx<user>(schema, ... [](const shared_ptr<user> &record) {
      cout << "User: " << record->to_string() << endl;
  });

  auto results = find_xxx<user>(schema, ...);

  for (auto user : results) {
      cout << "User: " << record->to_string() << endl;
  }
```

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
    user obj;

    obj.set_id(1);

    if(!obj.de1ete()) {
        cerr << testdb.last_error() << endl;
    }
```


Prepared Statements
===================

Indexed binding parameters in queries can use the dollar sign syntax:

```c++
 "$1, $2, $3, $1, etc"
```

or the ? syntax:

```c++
 "?, ?, ?, ?, etc"
```

Named parameters are also supported using a '@' or ':' prefix:

```c++
  "id = @id, name = :name, etc."
```

You **can** mix indexed and named parameters.

```c++
  "?, $2, @name, $3"
  // or
  "?, ?, @name, ?"
```

When mixing indexed parameters, the first '?' is equivelent to parameter 1 or '$1' and so on.
Mixing parameter types is an area that has been tested, but nearly enough (03/13/16).

Binding
-------

The binding interface looks like this:

```c++
// Bind all by order (index)
query.bind_all("value1", "value2", value3);

// or
query.where("param = $1, param2 = $2", value1, value2);

// Bind by index
query.bind(2, value2);

// Bind by name
query.bind("@param", "value");

// Bind by generic type
sql_value value(1234);
query.bind_value(1, value);
```

Basic Queries
=============

Modify Queries
--------------
```c++
/* insert a  user (INSERT INTO ...) */
insert_query insert(current_session);

/* insert column values into a table */
insert.into("users").columns({"id", "first_name", "last_name"})
      .values(4321, "dave", "patterson");

if (!query.execute()) {
    cerr << testdb.last_error() << endl;
} else {
    cout << "last insert id " << query.last_insert_id() << endl;
}
```

```c++
/* update a user (UPDATE ...) */
update_query update(current_session);

/* update columns in a table with values */
update.table("users").columns({"id", "first_name", "last_name"})
     .values(3432, "mark", "anthony");

/* using where clause with named parameters */
query.where("id = @id") or ("last_name = @last_name");

/* bind named parameters */
query.bind("@id", 1234).bind("@last_name", "henry");

query.execute();
```

```c++
/* delete a user (DELETE FROM ...) */
delete_query query(current_session);

query.from("users").where("id = $1 AND first_name = $2", 1234, "bob");

query.execute();

```

Select Query
------------

```c++
/* select some users */
select_query query(current_session);

query.from("users").where("last_name = $1 OR first_name = $2", "Jenkins", "Harry");

auto results = query.execute();

for ( auto &row : results) {
    string lName = row["last_name"]; // "Jenkins"
    // do more stuff
}

```

The select query also supports a call back interface:

```c++
select_query query(current_session);

query.from("users").execute([](const resultset & rs)
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

The **join_clause** is used to build join statements.

```c++

select_query select(current_session, {"u.id", "s.setting"});

select.from("users u").join("user_settings s").on("u.id = s.user_id") and ("s.valid = 1");

select.execute();

```

Where Clauses
-------------

Where clauses in select/delete/joins have a dedicated class. For me it is syntactically preferrable to use the 'and' and 'or' keywords with the where clauses operators.

```c++
query.where("this = $1") and ("that = $2") or ("test = $3");
```

The library will try to put the appropriate combined AND/OR into brackets itself. In the above example it would result in:

```
(this = $1 AND that = $2) OR (test = $3)
```

This is also an area that has been tested, but not nearly enough (03/13/16).

Batch Queries
-------------

Batch queries means that instead of releasing the resources of a query upon execution, its will reset them to a pre-bind state.

```c++
/* execute some raw sql */
insert_query insert(current_session);

insert.into("users").columns({"counter"});

/* turn on batch mode for this query */
insert.flags(insert_query::Batch);

for(int i = 1000; i < 3000; i++) {
    insert.bind(1, i);

    if (!insert.execute()) {
        cerr << testdb.last_error() << endl;
    }
}
```

Transactions
============

Transaction can be performed on a session object. 

```c++
{
  auto tx = current_session->start_transaction();

  /* perform operations here */

  tx->save("savepoint");

  /* more operations here */

  tx->rollback("savepoint");
  // tx->release("savepoint");

  // set successful to commit on destruct
  tx->set_successful(true);
}
// tx will be commited here
```

Types
=====

A [variant](http://github.com/ryjen/arg3variant) class is used for converting and storing data types. A few custom types exist:

sql_time
--------

A type for dealing with sql date/time formats.

```c++
time_t current_time = time(0);

/*
 * create a DATE sql value
 *
 * can be DATE, TIME, DATETIME, TIMESTAMP
 */
sql_time value(current_time, sql_time::DATE);

/* binds the date to a query */
query.bind(1, value);

/* YYYY-MM-DD format */
auto str = value.to_string();
```

sql_blob
--------

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

For sqlite3 and mysql databases, results from a query will be limited to the scope of the statement.

Memory caching was add to pre-fetch the values (sqlite only).  It can be done at the resultset, row or column level.  The default is none.

```c++
  auto sqlite_session = sqldb::create_session<sqlite::session>("sqlite://testdb.db");
  sqlite_session->cache_level(sqlite::cache::row);
```

Mysql has pre-fetching built-in and it is used within the library.  It is enabled by default. 

an example of turning caching off in mysql:
```c++
  auto mysql_session = sqldb::create_session<mysql::session>("mysql://localhost/test");
  mysql_session->flags(mydb.flags() & ~mysql::db::CACHE);
```

Memory caching is also used for looking up schemas to reduce hits to the database.

Alternatives
============

- [Poco Data](http://pocoproject.org/docs/00200-DataUserManual.html)
- [SQLAPI++](http://www.sqlapi.com)
- [SOCI](http://soci.sourceforge.net)

TODO / ROADMAP
==============

* More and better quality tests, I demand 100% coverage
* compare benchmarks with other libraries

[![BitCoin donate button](https://img.shields.io/badge/bitcoin-donate-yellow.svg)](https://coinbase.com/checkouts/9ef59f5479eec1d97d63382c9ebcb93a "Donate once-off to this project using BitCoin")
Buy me a beer! 




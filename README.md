
coda_db
=====

[![Build Status](http://img.shields.io/travis/ryjen/db.svg)](https://travis-ci.org/ryjen/db)
[![Coverage Status](https://coveralls.io/repos/ryjen/db/badge.svg?branch=master&service=github)](https://coveralls.io/github/ryjen/db?branch=master)
[![License](http://img.shields.io/:license-gpl.v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0.en.html)
[![Code Grade](https://img.shields.io/codacy/grade/e98c311926b94b068ef6705245d77739.svg)](https://www.codacy.com/app/ryjen/coda_db/dashboard)
[![Beer Pay](https://img.shields.io/beerpay/ryjen/db.svg)](https://beerpay.io/ryjen/db)

a sqlite, mysql and postgres api with active record (ish) implementation.   


Building
--------

After cloning run the following command to initialize submodules:

```bash
git submodule update --init --recursive
```

[Docker](https://www.docker.com) builds are available,
docker-compose will run the tests with mysql and postgres images:

```c++
docker-compose run test
```

otherwise use [cmake](https://cmake.org) to generate for the build system of your choice, including Xcode.

```bash
cmake -G Xcode .
open coda_db.xcodeproj
```

options supported are:

```
-DENABLE_COVERAGE=OFF            : enable code coverage using lcov
-DENABLE_MEMCHECK=OFF            : enable valgrind memory checking on tests
-DENABLE_PROFILING=OFF           : enable valgrind profiling on tests
-DENABLE_PARAMETER_MAPPING=OFF   : use regex to map different parameter syntaxes
-DENABLE_BENCHMARKING=OFF        : benchmark with other database libraries
```

Debugging
---------

Debugging on docker can be done with docker compose:

```
docker-compose run test gdb /usr/src/docker-build/tests/coda_db_test_xxx
```



Documentation
--------------

The complete API documentation is available at [https://ryjen.github.io/db](https://ryjen.github.io/db)

View some [diagrams here](https://github.com/ryjen/db/wiki/Model).  These need to be improved.

Records
=======

An user object example
----------------------

For the purposes of this readme there is a global session variable:

```c++
std::shared_ptr<coda::db::session> current_session = coda::db::create_session("file://test.db");

/* Other databases:

current_session = coda::db::create_session("mysql://user@pass:localhost:3306/database");
current_session = coda::db::create_session("postgres://localhost/test");

*/
```

Record objects should be implemented using the [curiously re-occuring template pattern (CRTP)](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern).

```c++

class user : public coda::db::record<user>
{
public:
    constexpr static const char *const TABLE_NAME = "users";

    /* only required constructor */
    user(const std::shared_ptr<schema> &schema) : record(schema)
    {}

    /* default constructor that gets the schema from the session */
    user(const std::shared_ptr<session> &session = current_session) 
          : record(session->get_schema(TABLE_NAME))
    {}

    /* utility method showing how to get columns */
    string to_string() const
    {
        ostringstream buf;
        buf << id() << ": " << get("first_name") << " " << get("last_name");
        return buf.str();
    }

    // optional overridden method to do custom initialization
    void on_record_init(const coda::db::row &row) {
        set("customValue", row.column("customName").to_value());
    }

    // custom find method using the schema functions
    vector<shared_ptr<user>> find_by_first_name(const string &value) {
        return coda::db::find_by<user>(this->schema(), "first_name", value);
    }
};
```

Save a record
-------------

```c++
/* save a user */
user obj;

obj.set("first_name", "John");
obj.set("last_name", "Doe");

if(!obj.save()) {
    cerr << current_session.last_error() << endl;
}
```

Delete a record
---------------

```c++
user obj;

obj.set_id(1);

if(!obj.remove()) {
    cerr << current_session.last_error() << endl;
}
```


Query a record
--------------

```c++
/* find users with a callback */
user().find_by_id(1234, [](const shared_ptr<user> &record) {
    cout << "User: " << record->to_string() << endl;
});

/* find users returning the results */
auto results = user().find_all();

for (auto &user : results) {
    cout << "User: " << record->to_string() << endl;
}
```


Querying Schemas
----------------

The library includes the following "schema functions" for querying with a schema objects:

- **find_by_id()**
- **find_all()**
- **find_by()**
- **find_one()**

These functions can:

- be generic column/values or specify type
- return results in a collection or use callback

example using a *callback* for a specific *user record*:
```c++
auto schema = current_session->get_schema(user::TABLE_NAME);

find_by_id<user>(schema, 1234, [](const shared_ptr<user> &record) {
    cout << "User: " << record->to_string() << endl;
});
```

example using a *return value* for a *generic record*:

```c++
auto results = find_all(schema);

for (auto record : results) {
    cout << "Obj: " << record->to_string() << endl;
}
```

Basic Queries
=============

Modify Queries
--------------

```c++
/* insert a  user (INSERT INTO ...) */
insert_query insert(current_session);

/* insert column values into a table */
insert.into("users").columns("id", "first_name", "last_name")
			.values(4321, "dave", "patterson");

if (!query.execute()) {
    cerr << current_session.last_error() << endl;
} else {
    cout << "last insert id " << query.last_insert_id() << endl;
}
```

```c++
/* update a user (UPDATE ...) */
update_query update(current_session);

/* update columns in a table with values */
update.table("users").columns("id", "first_name", "last_name")
		 .values(3432, "mark", "anthony");

/* using where clause with named parameters */
query.where(op::equals("id", 1234)) or op::equals("last_name", "henry");

query.execute();
```

```c++
/* delete a user (DELETE FROM ...) */
delete_query query(current_session);

query.from("users").where(equals("id", 1234)) and equals("first_name", "bob");

query.execute();

```

Select Query
------------

```c++
/* select some users */
select_query query(current_session);

query.from("users").where(equals("last_name", "Jenkins")) or equals("first_name", "Harry");

auto results = query.execute();

for ( auto &row : results) {
    string lName = row["last_name"];
    // do more stuff
}
```

The select query also supports a call back interface:

```c++
select_query query(current_session);

query.from("users").execute([](const resultset & rs)
{
    // do something with a resultset

    rs.each([](const row & r)
    {
        // do something with a row

        r.each([](const column & c)
        {
                // do something with a column
        });
    });
});

// use a function for a callback
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
select_query select(current_session);

select.columns("u.id", "s.setting").from("users u")
    .join("user_settings s").on("u.id = s.user_id") and ("s.valid = 1");

select.execute();
```

Raw Queries
-----------

Perform raw queries on a session object if you have to:

```c++
auto results = session->query("select * from users");

if (!session->execute("insert into users values(...)")) {
    cerr << session->last_error() << endl;
}
```
Transactions
============

Transactions can be performed on a session object.

```c++
...
{
	auto tx = current_session->start_transaction();

	/* perform operations here */

	tx->save("savepoint");

	/* more operations here */

	tx->rollback("savepoint");
	tx->release("savepoint");

	// set successful to commit on destruct
	tx->set_successful(true);
}
// tx will be commited here
...
```

Prepared Statements
===================

#### Parameters

By default the library will use the prepared statement syntax of the database being used.  This is the most efficient use.

*****

###### **ENHANCED_PARAMENTER_MAPPING (experimental feature)**
 
If you compile with this flag, then the following syntaxes are managed by the library.

- **$1, $2**, etc :
   - indexed parameter
- **?** :
   - ordered index parameter (the first ? is equivalent to $1 and so on)
- **@name** :
   - named paramters separate from indexed parameters

With this flag you can combine the different syntaxes.... in the same query if you really wanted.

Example:

```c++
"?, $2, @name, $3"
// or
"?, ?, @name, ?"
```

#### Behaviour

Upon execution, the queries will be reset to a pre-bind state.

```c++
/* execute some raw sql */
insert_query insert(current_session);

insert.into("users").columns("counter");

for(int i = 1000; i < 3000; i++) {
    // set new values for the insert
    insert.bind(1, i);

    if (!insert.execute()) {
            cerr << current_session.last_error() << endl;
    }
}
```


Where Clauses / Binding
-----------------------

Where clauses in select/delete/join queries have a dedicated class.

```c++
// using a where clause builder
query.where(equals("param1", value1)) and !in("param2", {24, 54}) or startswith("param3", "abc");
```

('and' and 'or' keywords are equivalent to the && || operators in c++)

The above example would produce the following SQL in postgres:

```
(param1 = $1 AND param2 NOT IN ($2,$3)) OR (param3 like $4)
```

Operator Helpers
================

Operator functions are used for where clauses:

```c++
query.where(equals("column", value)) and !between("column2", valueA, valueB);
```

```c++
op::equals
op::like
op::startswith
op::endswith
op::contains
op::in
op::between
op::is
```

They can all be negated using the operator! (ex. !like ) 

Types
=====

sql_value is the base type and implemented using a variant (currently boost::variant, until c++17 variant becomes available).

sql_value is capable of converting between the basic SQL values (when possible).

Subtypes include:

sql_string
----------
defined as std::string

sql_wstring
-----------
defined as std::wstring

sql_null
-------- 
defined as nullptr

sql_number
----------

A custom type for storing and converting numeric values only.

sql_time
--------

A type for storing and converting sql date/time formats.

```c++
time_t current_time = time(0);

/*
 * create a DATE sql value
 *
 * can be DATE, TIME, DATETIME, TIMESTAMP
 */
sql_time value(current_time, sql_time::DATE);

/* YYYY-MM-DD format */
auto str = value.to_string();
```

sql_blob
--------

currently defined as a vector of bytes.

```c++
/* set data here */
void *data = malloc(sz);

/*
 * create a blob value, this will create a copy of the data
 *
 * you can pass function pointers to control how the data is allocated, freed, copied and compared
 */
sql_blob value(data, data + sz);

insert.into("table").columns("blob").values(value);
```

Benchmarking
============

Here are some preliminary benchmarks on sqlite (see [tests/benchmarks](tests/benchmarks)).  Tested on mac osx pro (2.5ghz), clang, release build.

	%> make benchmark
	Scanning dependencies of target benchmark
	Executing benchmarks...
	sqlite insert                              5000      437175 ns/op
	sqlite select                              2000      587871 ns/op
	coda_db/coda_db_benchmark 7.204s
	sqlite insert                              5000      417281 ns/op
	sqlite select                              2000     1245470 ns/op
	poco/coda_db_benchmark_poco 8.651s
	sqlite insert                              5000      437498 ns/op
	sqlite select                              2000     1274096 ns/op
	soci/coda_db_benchmark_soci 9.018s
	Built target benchmark

Why another library?
--------------------

Why not? It was good fun, tested and usable.  

Other libraries are kinda nice or even better at some things.  Sometimes you don't want to deal with code generators or unintuitive syntax though.

Alternatives
============
- [sqlpp11](https://github.com/rbock/sqlpp11)
- [sqlite3pp](https://github.com/iwongu/sqlite3pp)
- [Poco Data](http://pocoproject.org/docs/00200-DataUserManual.html)
- [SQLAPI++](http://www.sqlapi.com)
- [SOCI](http://soci.sourceforge.net)

TODO / ROADMAP
==============

[ ] Replace exceptions in favor of return types
[ ] Custom where clause operators
[ ] More testing around binding
[ ] More testing around data types
[ ] More testing with where clauses
[ ] Better benchmarking and perf improvements
[ ] Finish/improve fuzz testing



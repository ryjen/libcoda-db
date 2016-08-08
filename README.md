
rj_db
=====

[![Build Status](http://img.shields.io/travis/ryjen/db.svg)](https://travis-ci.org/ryjen/db)
[![Coverage Status](https://coveralls.io/repos/ryjen/db/badge.svg?branch=master&service=github)](https://coveralls.io/github/ryjen/db?branch=master)
[![License](http://img.shields.io/:license-gpl.v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0.en.html)
[![Code Grade](https://img.shields.io/codacy/grade/e98c311926b94b068ef6705245d77739.svg)](https://www.codacy.com/app/ryjen/rj_db/dashboard)
[![Beer Pay](https://img.shields.io/beerpay/ryjen/db.svg)](https://beerpay.io/ryjen/db)

a sqlite, mysql and postgres wrapper / active record (ish) implementation.   use in production at your own risk, no support or warrenty.

Why another library
-------------------

Mostly for the challenge and to use newer features of c++11 in a database context.  Other libraries have added C++11 after the fact.

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

otherwise use [cmake](https://cmake.org) to generate for the build system of your choice.

```bash
mkdir debug; cd debug;
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
make test
```

options supported are:

		-DCODE_COVERAGE=OFF              : enable code coverage using lcov
		-DMEMORY_CHECK=OFF               : enable valgrind memory checking on tests
		-DENABLE_LOGGING=OFF             : enable internal library logging
		-DENABLE_PROFILING=OFF           : enable valgrind profiling on tests
		-DENHANCED_PARAMETER_MAPPING=OFF : use regex to map different parameter syntaxes
		-DENABLE_BENCHMARKING=OFF        : benchmark with other database libraries


Debugging
---------

Debugging on docker can be done with docker compose:

```
docker-compose run test gdb /usr/src/build/tests/rj_db_test_xxx
```

Model
-----

![session interface](db_sessions.png)

![query interface](db_query.png)

![results interface](db_results.png)

Records
=======

An simple user example
----------------------

Records should be implemented using the [curiously re-occuring template pattern (CRTP)](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern).

First initialize a session
```c++
auto current_session = sqldb::create_session("file://test.db");

/* Other databases:

auto current_session = sqldb::create_session("mysql://user@pass:localhost:3306/database");
auto current_session = sqldb::create_session("postgres://localhost/test");

*/
```

Create a record
```c++

extern std::shared_ptr<rj::db::session> current_session;

class user : public rj::db::record<user>
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
		void on_record_init(const rj::db::row &row) {
				set("customValue", row.column("customName").to_value());
		}

		// custom find method using the schema functions
		vector<shared_ptr<user>> find_by_first_name(const string &value) {
				return rj::db::find_by<user>(this->schema(), "first_name", value);
		}
};
```

Querying records
----------------

The library includes the following schema functions for querying:

- **find_by_id()**
- **find_all()**
- **find_by()**
- **find_one()**

example using a callback:
```c++
	auto schema = current_session->get_schema(user::TABLE_NAME);

	find_xxx<user>(schema, ... [](const shared_ptr<user> &record) {
			cout << "User: " << record->to_string() << endl;
	});
```

example using a return value:

```c++
	auto results = find_xxx<user>(schema, ...);

	for (auto user : results) {
			cout << "User: " << record->to_string() << endl;
	}
```

Record objects have their equivalent methods using their internal schema:

```c++
	/* find users with a callback */
	user().find_xxx(... [](const shared_ptr<user> &record) {
			cout << "User: " << record->to_string() << endl;
	});

	/* find users returning the results */
	auto results = user().find_xxx(...);

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

By default and for performance, the library will use the prepared statement syntax of the database being used.

If you turn on ENHANCED_PARAMENTER_MAPPING at compile time, then the syntaxes are universal - including named parameters and mixing parameter syntaxes.

Enhanced parameter mapping example:

```c++
	"?, $2, @name, $3"
	// or
	"?, ?, @name, ?"
```

When mixing indexed parameters, the first '?' is equivalent to parameter 1 or '$1' and so on.
Mixing parameter types is an area that has been tested, but nearly enough (03/13/16).

Binding
-------

The binding interface looks like this:

```c++
// Bind all by order (index)
query.bind_all("value1", "value2", value3);

// or
query.where("param = $1 and param2 = $2", value1, value2);

// Bind by index
query.bind(2, value2);

// Bind by name
query.bind("@param", "value");

// Bind by generic type
sql_value value(1234);
query.bind_value(1, value);

// Bind by vector of values
vector<sql_value> values = { 1234, "bob", "smith" };
query.bind(values);

// Bind by a map of named values
unordered_map<string,sql_value> values = { {"@name", "harry"}, {"@id", 1234} };
query.bind(values);
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
		cerr << testdb.last_error() << endl;
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

		rs.for_each([](const row & r)
		{
				// do something with a row

				r.for_each([](const column & c)
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

Where Clauses
-------------

Where clauses in select/delete/joins have a dedicated class. For me it is syntactically preferable to use the 'and' and 'or' keywords with the where clause operators.  This is the same as calling the && || operators.

```c++
query.where("this = $1") and ("that = $2") or ("test = $3");
```

The library will try to put the appropriate combined AND/OR into brackets itself. In the above example it would result in:

```
(this = $1 AND that = $2) OR (test = $3)
```

Grouping where clauses is also an area that has been tested, but not nearly enough (03/13/16).

Batch Queries
-------------

The library supports batch mode by default. This means upon execution, its will reset the query to a pre-bind state.

```c++
/* execute some raw sql */
insert_query insert(current_session);

insert.into("users").columns("counter");

for(int i = 1000; i < 3000; i++) {
		// set new values for the insert
		insert.bind(1, i);

		if (!insert.execute()) {
				cerr << testdb.last_error() << endl;
		}
}
```

Raw Queries
-----------

Perform raw queries on a session object:

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
```

Types
=====

A [variant](http://github.com/ryjen/variant) class is used for converting and storing data types. A few custom types exist:

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

Additional custom types can be implemented by subclassing **variant::complex**. For example, the JSON postgres type.

Benchmarking
============

Here are some preliminary benchmarks on sqlite (see tests/benchmarks).  Tested on mac osx pro using clang release mode.

	sqlite insert                              5000      406684 ns/op
	sqlite select                              2000     1841120 ns/op
	tests/benchmarks/rj_db/rj_db_benchmark 10.182s

	sqlite insert                              5000      409861 ns/op
	sqlite select                              2000     1560117 ns/op
	tests/benchmarks/poco/rj_db_benchmark_poco 9.407s

	sqlite insert                              5000      403932 ns/op
	sqlite select                              2000     1430914 ns/op
	tests/benchmarks/soci/rj_db_benchmark_soci 9.082s

Alternatives
============

- [Poco Data](http://pocoproject.org/docs/00200-DataUserManual.html)
- [SQLAPI++](http://www.sqlapi.com)
- [SOCI](http://soci.sourceforge.net)

TODO / ROADMAP
==============

* More and better quality tests, I demand 100% coverage
* cbetter benchmarking and perf improvements
* NoSQL support? might be doable


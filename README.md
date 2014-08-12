
libarg3db
=========

a sqlite3 and mysql wrapper / active record (ish) implementation

[View Testing Code Coverage](http://htmlpreview.github.com/?https://github.com/c0der78/arg3db/blob/master/coverage/index.html)

Building
========

I use [autotools](http://en.wikipedia.org/wiki/GNU_build_system).

```bash
./configure --prefix=/usr/local

make
```

Coding Style
============

- class/struct/method names are all lower case with underscores separating words
- non public members are camel case with and underscore at end of the name
- macros, enums and constants are all upper case with underscores seperating words
- braces on a new line

Model
=====
<pre>
/* database interfaces */
<b>sqldb</b>                                 - interface for a specific database
  └ <b>statement</b>                         - interface for a prepared statement
        └ <b>resultset</b>                   - results of a statement
              └ <b>row</b>                   - an single result
                   └ <b>column</b>           - a field in a row containing a value

/* implementations using the above*/
<b>schema</b>                                - a definition of a table
<b>schema_factory</b>                        - cached schemas
<b>base_record</b>                           - the active record (ish) implementation
<b>select_query</b>                          - builds select queries
<b>modify_query</b>                          - builds update/insert queries
<b>delete_query</b>                          - builds delete queries
<b>sql_value</b>                             - storage and conversion for basic sql types
</pre>

Records
=======

Base Record
-----------
```c++
arg3::db::sqlite3_db testdb("test.db");
//arg3::db::mysql_db testdb("database", "user", "password", "localhost", 3306);

class user : public arg3::db::base_record<user>
{
    constexpr static const char *const ID_COLUMN = "id";
    constexpr static const char *const TABLE_NAME = "users";
public:
    /* default constructor */
    user() : base_record(&testdb, TABLE_NAME, ID_COLUMN) {}

    /* results constructor */
    user(const row &values) : base_record(&testdb, TABLE_NAME, ID_COLUMN, values) {}

    /* id constructor */
    user(long id) : base_record(&testdb, TABLE_NAME, ID_COLUMN, id) {}

    /* utility method showing how to get columns */
    string to_string() const
    {
        ostringstream buf;

        buf << id() << ": " << get("first_name") << " " << get("last_name");

        return buf.str();
    }
};
```

Query records
-------------
```c++
    /* find all users */
 	user().find_all([](shared_ptr<user> record) {
        cout << "User: " << record->to_string() << endl;
    }

    /* alternative type returns a vector */
    results = user().find_by("first_name", "Joe");

    for (auto user : results)
    {
        cout << "Found user: " << user->to_string() << endl;
    }
````
Save a record
-------------
```c++
    /* save a user */
    user obj;

    obj.set("first_name", "John");
    obj.set("last_name", "Doe");

    if(!obj.save())
    	cerr << testdb.last_error() << endl;
```

Delete a record
---------------
```c++
    user obj(1); // id constructor

    if(!obj.de1ete())
        cerr << testdb.last_error() << endl;
```

Basic Queries
=============

Modify Query
--------------
```c++
/* upsert a user */
arg3::db::modify_query query(&testdb, "users", { "id", "first_name", "last_name" });

query.bind(1, 1234).bind(2, "happy").bind(3, "gilmour");

/* saves user { "id": 1234, "first_name": "happy", "last_name": "gilmour" } */
if(!query.execute())
    cerr << testdb.last_error() << endl;
```

Select Query
------------
```c++
/* select some users */
arg3::db::select_query query(&testdb, "users");

query.where("last_name = ?");

query.bind(1, "Jenkins");

auto results = query.execute();

for ( auto &row : results) {
    string lName = row["last_name"]; // "Jenkins"
    ...
}

/* alternatively */
typedef std::function<void (const resultset &)> handler_type;

handler_type handler = [](const resultset &results)
{
    results.for_each([](const row &row) {
        string lName = row["last_name"]; // "Jenkins"
        ...
    });
}
query.execute(handler);
```


TODO
====

* More tests
* More database implementations



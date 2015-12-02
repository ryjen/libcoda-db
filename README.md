
libarg3db
=========

[![Build Status](https://travis-ci.org/deadcoda/arg3db.svg?branch=master)](https://travis-ci.org/deadcoda/arg3db)

a sqlite3 and mysql wrapper / active record (ish) implementation

[View Testing Code Coverage](http://htmlpreview.github.com/?https://github.com/c0der78/arg3db/blob/master/coverage/index.html)

Use in production at your own risk, I'm still doing quite a bit of testing, refactoring and new features.  Pull requests are welcomed...

I do however realize already that this is most likely an excercise in futility.

Building
========

You can use [cmake](http://cmake.org) or the legacy [autotools](http://en.wikipedia.org/wiki/GNU_build_system) systems to build.

```bash
mkdir debug
cd debug

# where to install (customize)
INSTALL_PREFIX=$(brew --cellar)/arg3db/0.5.0

# use cmake for debug build with valgrind, lcov
cmake -DMEMORY_CHECK=ON -DCODE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX ..
# or autotools
./configure --prefix=$INSTALL_PREFIX --enable-coverage

make
make test
make install

# osx homebrew
brew link arg3db
```

Coding Style
============

I don't like camel case classes/structs/methods/functions in c++.  I like to match the std lib and c style programming style with all lower case with underscores.  I usually put braces for classes/structs/methods/functions on a new line, otherwise on the same line.  Globals, constants, enums are uppercase underscored.  Private member variables end with an open underscore.

Model
=====

            /* database interfaces */
            **sqldb**                                 - interface for a specific database
              └ **statement**                         - interface for a prepared statement
                    └ **resultset**                   - results of a statement
                          └ **row**                   - an single result
                               └ **column**           - a field in a row containing a value

            /* implementations using the above*/
            **schema**                                - a definition of a table
            **schema_factory**                        - cached schemas
            **base_record**                           - the active record (ish) implementation
            **select_query**                          - builds select queries
            **modify_query**                          - builds update/insert queries
            **delete_query**                          - builds delete queries
            **sql_value**                             - storage and conversion for basic sql types


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
    /* default constructor, no database hits */
    user() : base_record(&testdb, TABLE_NAME, ID_COLUMN) {}

    /* results constructor */
    user(const row &values) : base_record(&testdb, TABLE_NAME, ID_COLUMN, values) {}

    /* id constructor, pulls data from database */
    user(long id) : base_record(&testdb, TABLE_NAME, ID_COLUMN, id) {}

    /* utility method showing how to get columns */
    string to_string() const
    {
        ostringstream buf;

        buf << id() << ": " << get("first_name") << " " << get("last_name");

        return buf.str();
    }

    // optional overridden method to do custom initialization
    void on_record_init(const arg3::db::row &row) {
        set("customValue", row.co1umn("customName").to_value());
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
    /* find all users */
 	user().find_all([](const shared_ptr<user> &record) {
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

Modify Queries
--------------
```c++
/* replace a user (REPLACE INTO ..) */
arg3::db::modify_query query(&testdb, "users", { "id", "first_name", "last_name" });

query.bind(1, 1234).bind(2, "happy").bind(3, "gilmour");

/* saves user { "id": 1234, "first_name": "happy", "last_name": "gilmour" } */
query.execute();


/* insert a  user (INSERT INTO ..) */
arg3::db::insert_query query(&testdb, "users"); /* auto find columns */

/* this would be the column order in table, TODO: named parameter binding */
query.bind(1, 4321).bind(2, "dave").bind(3, "patterson");

if (!query.execute())
    cerr << testdb.last_error() << endl;
else
    cout << "last insert id " << query.last_insert_id() << endl;


/* update a user (UPDATE ...) */
arg3::db::update_query query(&testdb, "users");

query.where("id = ?");

query.bind(1, 3432).bind(2, "mark").bind(3, "anthony").bind(4, 1234);

query.execute();


/* delete a user (DELETE FROM ...) */
arg3::db::delete_query query(&testdb, "users");

query.where("id = ?");

query.bind(1, 1234);

query.execute();

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
    // do more stuff
}

/* alternatively */

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


Batch Queries
-------------
```c++
/* execute some raw sql */
arg3::db::insert_query query(&testdb, "counts");

/* turn on batch mode for this query */
query.set_flags(modify_query::BATCH);

for(int i = 1000; i < 3000; i++) {
    query.bind(1, i);

    if (!query.execute()) {
        cerr << testdb.last_error() << endl;
    }
}
```

Alternatives
============

- [Poco Data](http://pocoproject.org/docs/00200-DataUserManual.html)
- [SQLAPI++](http://www.sqlapi.com)
- [SOCI](http://soci.sourceforge.net)

TODO / ROADMAP
==============

* named parameter indexing, especially for column names (col_name = ?col_name, bind("col_name", value))
* More database implementations (postgres!)
* More tests, at least 95% test coverage



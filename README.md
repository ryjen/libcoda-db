
libarg3db
=========

a sqlite3 wrapper / active record (ish) implementation

Building
========

*OSX:*

[Download](http://brew.sh) and install homebrew.

```bash
brew install premake

premake4 gmake

make
```

*Windows:*

- [Download](http://industriousone.com/premake/download) premake.
- run <code>premake4 vs2010</code>.
- Open the generated project file in Visual Studio.


Model
=====
<pre>
/* database specific */
<b>sqldb</b>                                 - interface for a specific database
  |- <b>statement</b>                        - interface for a prepared statement
        |- <b>resultset</b>                  - results of a statement
              |- <b>row</b>                  - an single result
                   |- <b>column</b>          - a field in a row containing a value

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

User Record
-----------
```c++
sqlite3_db testdb("test.db");
//mysql_db testdb("database", "user", "password", "localhost", 3306);

class user : public base_record<user>
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
    /* get all users */
 	auto results = user().find_all();

    for (auto &user : results)
    {
        cout << "User: " << user.to_string() << endl;
    }

    results = user().find_by("first_name", "Joe");

    for (auto &user : results)
    {
        cout << "Found user: " << user.to_string() << endl;
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
    	cout << testdb.last_error() << endl;
```

Delete a record
---------------
```c++
    user obj(1); // id constructor

    if(!obj.de1ete())
        cout << testdb.last_error() << endl;
```

Basic Queries
=============

Modify Query
--------------
```c++
/* upsert a user */
modify_query query(&testdb, "users", { "id", "first_name", "last_name" });

query.bind(1, 1234).bind(2, "happy").bind(3, "gilmour");

if(!query.execute())
    cout << testdb.last_error() << endl;
```

Select Query
------------
```c++
/* select some users */
select_query query(&testdb, "users");

query.where("last_name = ?");

query.bind(1, "Jenkins");

auto results = query.execute();

for(auto &row: results)
{
    string fName = row["first_name"];
    ...
}
```

TODO
====

* More tests
* More database implementations




libarg3db
=========

a sqlite3 wrapper / active record (ish) implementation

Building
========

```bash
brew install premake

premake4 gmake/vs2010

make
```

Model
=====
<pre>
/* database specific */
<b>sqldb</b>                                 - implementation of a specific database
  |- <b>statement</b>                        - implementation of a prepared statement
        |- <b>resultset</b>                  - results of a statement
              |- <b>row</b>                  - an entry of a table
                   |- <b>column</b>          - a value continer

/* implementations based on above*/
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

class user : public base_record<user>
{
    constexpr static const char *ID_COLUMN = "id";
    constexpr static const char *TABLE_NAME = "users";
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

    for (auto &obj : results)
    {
        cout << "Loaded " << obj.to_string() << endl;
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
select_query query(testdb, "users");

query.where("last_name = ?");

query.bind(1, "Jenkins");

auto results = query.execute();

user jenkins(*results);
```

TODO
====

* More tests


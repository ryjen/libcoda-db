
libarg3db
=========

a sqlite3 wrapper / active record (ish) implementation

Building
========

```bash
brew install premake

premake4 gmake

make
```

Records
=======

base_record.h provides a ORM type functionality.  Records look for an id column based on the table name.

a sample model object:
```c++
sqlite3_db testdb("test.db");

class user : public base_record
{
    constexpr static const char *ID_COLUMN = "id";
    constexpr static const char *TABLE_NAME = "users";
public:
    /* default constructor */
    user() : base_record(&testdb, TABLE_NAME, ID_COLUMN) {}

    /* required query constructor */
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

and using the user object:
```c++
    /* get all users */
 	auto results = user().find_all();

    for (auto &obj : results)
    {
        cout << "Loaded " << obj.to_string() << endl;
    }

    user obj;

    obj.set("first_name", "Jim");
    obj.set("last_name", "Bob");

    if(!obj.save())
    	cout << obj.db().last_error() << endl;

```


Queries: Select and Modify
==========================

```c++
sqldb testdb("test.db");

/* upsert a user */
modify_query query(&testdb, "users", { "id", "first_name", "last_name" });

query.bind(1, 1234).bind(2, "happy").bind(3, "gilmour");

if(!query.execute())
    cout << testdb.last_error() << endl;

/* select some users */
select_query query(testdb, "users");

query.where("last_name = ?");

query.bind(1, "Jenkins");

auto results = query.execute();
```

TODO
====

* More tests


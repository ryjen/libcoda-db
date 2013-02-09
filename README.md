
libarg3db
=========

a sqlite3 wrapper / active record implementation

a sample model object:
```c++
sqldb testdb("test.db");

class user : public base_record
{
public:
    user() {}

    user(const row &values) : base_record(values) {}

    column_definition columns() const
    {
        return
        {
            {"id",SQLITE_INTEGER },
            {"first_name",SQLITE_TEXT},
            {"last_name", SQLITE_TEXT}
        };
    }

    string tableName() const
    {
        return "users";
    }

    sqldb db() const
    {
        return testdb;
    }

    string to_string()
    {
        ostringstream buf;

        buf << get("id") << ": " << get("first_name") << " " << get("last_name");

        return buf.str();
    }

};
```

and using the user object:
```c++
 	auto results = user::findAll<user>();

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


licensing
=========

copyrighted under the Lesser GNU Public License

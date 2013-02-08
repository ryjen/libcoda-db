#include <igloo/igloo.h>
#include "sqldb.h"
#include "base_record.h"
#include "select_query.h"

using namespace igloo;

using namespace std;

using namespace arg3::db;

sqldb testdb("test.db");

class user : public base_record
{
public:
	user() {}

	user(const row &values) : base_record(values) {}

	column_definition columns() const {
		return {
			{"id",SQLITE_INTEGER },
			{"first_name",SQLITE_TEXT},
			{"last_name", SQLITE_TEXT}
		};
	}

	string tableName() const {
		return "users";
	}

	sqldb db() const {
		return testdb;
	}

	string to_string() {
		ostringstream buf;

		buf << get("id") << ": " << get("first_name") << " " << get("last_name");

		return buf.str();
	}

};

Context(sqldb_test)
{
	Spec(save_test)
	{
		try {
			testdb.open();

			user user1;

			testdb.execute("create table if not exists users(id integer primary key autoincrement, first_name varchar(45), last_name varchar(45))");

			user1.set("id", 1LL);
			user1.set("first_name", "Ryan");
			user1.set("last_name", "Jennings");

			cout << "Saving " << user1.to_string() << endl;

			if(!user1.save())
				cerr << "Error1: " << testdb.last_error() << endl;

			user1.set("first_name", "Bryan");
			user1.set("last_name", "Jenkins");

			cout << "Saving " << user1.to_string() << endl;

			if(!user1.save())
				cerr << "Error2: " << testdb.last_error() << endl;

			auto query2 = testdb.select("users");

			auto results = user::findAll<user>();

			for(auto user2 : results) {
				//user user2(row);

				cout << "Loaded " << user2.to_string() << endl;
			}

			testdb.close();
		}
		catch(const database_exception &e) {
			cerr << "Error3: " << testdb.last_error() << endl;
			throw e;
		}
	}
};


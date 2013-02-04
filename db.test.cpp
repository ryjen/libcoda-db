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

	user(const row &row) : base_record(row) {
	}

	columnset columns() const {
		return {"id","first_name","last_name"};
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

			user1.set("id", 1);
			user1.set("first_name", "Ryan");
			user1.set("last_name", "Jennings");

			user1.save();

			user1.set("first_name", "Bryan");
			user1.set("last_name", "Jenkins");

			user1.save();

			auto query2 = testdb.select("users");

			auto results = query2.execute();

			for(auto row : results) {
				user user2(row);

				cout << "User: " << user2.to_string() << endl;
			}

			testdb.close();
		}
		catch(const database_exception &e) {
			cerr << "Error: " << testdb.last_error() << endl;
			throw e;
		}
	}
};


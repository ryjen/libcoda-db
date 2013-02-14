/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include <igloo/igloo.h>
#include "sqldb.h"
#include "base_record.h"
#include "select_query.h"

using namespace igloo;

using namespace std;

using namespace arg3::db;

sqldb testdb("test.db");

class user : public id_record<int>
{
private:
    string idColumnName() const {
        return "id";
    }
public:
    user() {}

    user(int id) : id_record(id) {}

    user(const row &values) : id_record(values) {}

    column_definition columns() const
    {
        return
        {
            {"id", SQLITE_INTEGER },
            {"first_name", SQLITE_TEXT},
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

        buf << getId() << ": " << get("first_name") << " " << get("last_name");

        return buf.str();
    }

    vector<user> findAll() {
        return base_record::findAll<user>();
    }

    void initById(const long long id) {
        base_record::initBy<user>("id", id);
    }
};

Context(sqldb_test)
{
    void SetUp() {
        testdb.open();


        testdb.execute("create table if not exists users(id integer primary key autoincrement, first_name varchar(45), last_name varchar(45))");

    }

    void TearDown() {
        testdb.close();
    }

    Spec(save_test)
    {
        try
        {
            user user1;

            user1.set("id", 1);
            user1.set("first_name", "Ryan");
            user1.set("last_name", "Jennings");

            Assert::That(user1.save(), Equals(true));

            user1.initById(1); // load values back up from db

            Assert::That(user1.get("first_name"), Equals("Ryan"));

            user1.set("first_name", "Bryan");
            user1.set("last_name", "Jenkins");

            Assert::That(user1.save(), Equals(true));

            user1.initById(1); // load values back up from db

            Assert::That(user1.get("first_name"), Equals("Bryan"));

        }
        catch (const database_exception &e)
        {
            cerr << "Error3: " << testdb.last_error() << endl;
            throw e;
        }
    }

    Spec(is_valid_test)
    {

            user user1;

            user1.initById(1432123);

            Assert::That(user1.is_valid(), Equals(false));

    }

    Spec(where_test)
    {
        
            auto query = testdb.select("users");

            query.where("first_name=? OR last_name=?");

            query.bind(1, "Bryan");
            query.bind(2, "Jenkins");

            //cout << query.to_string() << endl;

            auto results = query.execute();

            auto row = results.begin();//results.first();

            Assert::That(row != results.end(), Equals(true));

            string lastName = row->column_value("last_name").to_string();

            Assert::That(lastName, Equals("Jenkins"));

    }
};


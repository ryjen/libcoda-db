#include "db.test.h"
#include "select_query.h"
#include "insert_query.h"
#include "log.h"

#include <chrono>

using namespace std;

using namespace arg3::db;

void run_tests(sqldb* db, const char* name);

#define TEST_SIZE 100000

int main(int argc, char* argv[])
{
    test_sqlite3_db sqlite_testdb;
    test_mysql_db mysql_testdb;
    test_postgres_db postgres_testdb;

    sqlite_testdb.setup();
    mysql_testdb.setup();
    postgres_testdb.setup();

    log::set_level(log::Info);

    run_tests(&sqlite_testdb, "sqlite");

    run_tests(&mysql_testdb, "mysql");

    run_tests(&postgres_testdb, "postgres");

    sqlite_testdb.teardown();
    mysql_testdb.teardown();
    postgres_testdb.teardown();

    return 0;
}

void run_tests(sqldb* db, const char* name)
{
    log::info("starting %s performance test...", name);

    auto t1 = std::chrono::high_resolution_clock::now();

    insert_query insert(db);

    insert.set_flags(insert_query::Batch);

    insert.into(user::TABLE_NAME).columns({"first_name", "last_name", "dval"});

    for (int i = 0; i < TEST_SIZE; i++) {
        insert.values(random_name(), random_name(), random_num<int>(-123012, 1231232));

        if (!insert.execute()) {
            log::error("unable to prepare test: %s", testdb->last_error().c_str());
            break;
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    log::info("\033[1;35m%s\033[0m inserts took: \033[1;37m%ld millis\033[0m", name,
              std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());

    t1 = std::chrono::high_resolution_clock::now();
    select_query select(db);

    select.from("users");

    for (auto& row : select.execute()) {
        if (!row.is_valid()) {
            log::error("row is invalid");
            break;
        }
    }
    t2 = std::chrono::high_resolution_clock::now();

    log::info("\033[1;35m%s\033[0m select all took: \033[1;37m%ld millis\033[0m", name,
              std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());
}
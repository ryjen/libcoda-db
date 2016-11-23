
#include "db.test.h"
#include "insert_query.h"
#include "log.h"
#include "select_query.h"

#include <chrono>

using namespace std;

using namespace rj::db;

void run_tests(const std::shared_ptr<rj::db::session>& session, const char* name);

#define TEST_SIZE 100000

int main(int argc, char* argv[])
{
    register_test_sessions();

    auto sqlite_test_session = sqldb::create_session("file://testdb.db");
    auto mysql_test_session = sqldb::create_session(get_env_uri("MYSQL_URI", "mysql://localhost/test"));
    auto postgres_test_session = sqldb::create_session(get_env_uri("POSTGRES_URI", "postgres://localhost/test"));

    auto sqlite_impl = dynamic_pointer_cast<test_session>(sqlite_test_session->impl());
    auto mysql_impl = dynamic_pointer_cast<test_session>(mysql_test_session->impl());
    auto postgres_impl = dynamic_pointer_cast<test_session>(postgres_test_session->impl());

    sqlite_impl->setup();
    mysql_impl->setup();
    postgres_impl->setup();

    log::set_level(log::Info);

    run_tests(sqlite_test_session, "sqlite");

    run_tests(mysql_test_session, "mysql");

    run_tests(postgres_test_session, "postgres");

    sqlite_impl->teardown();
    mysql_impl->teardown();
    postgres_impl->teardown();

    return 0;
}

void run_tests(const std::shared_ptr<rj::db::session>& session, const char* name)
{
    log::info("starting %s performance test...", name);

    auto t1 = std::chrono::high_resolution_clock::now();

    insert_query insert(session);

    insert.flags(insert_query::Batch);

    insert.into(user::TABLE_NAME).columns({"first_name", "last_name", "dval"});

    for (int i = 0; i < TEST_SIZE; i++) {
        insert.values(random_name(), random_name(), random_num<int>(-123012, 1231232));

        if (!insert.execute()) {
            log::error("unable to prepare test: %s", session->last_error().c_str());
            break;
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    log::info("\033[1;35m%s\033[0m inserts took: \033[1;37m%ld millis\033[0m", name,
              std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());

    t1 = std::chrono::high_resolution_clock::now();
    select_query select(session);

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

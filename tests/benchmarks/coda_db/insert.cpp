
#define BENCHPRESS_CONFIG_MAIN
#include <benchpress/benchpress.hpp>
#include "benchmark.h"
#include "log.h"
#include "sqlite/session.h"
#include "util.h"

using namespace coda::db;

BENCHMARK("sqlite insert", [](benchpress::context *context) {
    uri uri_s("file://test.db");

    benchmark_setup(uri_s);

    sqlite_setup();

    coda::db::insert_query query(current_session);

    query.into(user::TABLE_NAME).columns("first_name", "last_name", "dval");

    context->reset_timer();

    for (size_t i = 0; i < context->num_iterations(); i++) {
        benchmark_insert(query, current_session);
    }

    context->stop_timer();

    sqlite_teardown();

    benchmark_teardown();
});


BENCHMARK("mysql insert", [](benchpress::context *context) {

    auto uri_s = get_env_uri("MYSQL_URI", "mysql://localhost/test");

    benchmark_setup(uri_s);

    mysql_setup();

    coda::db::insert_query query(current_session);

    query.into(user::TABLE_NAME).columns("first_name", "last_name", "dval");

    context->reset_timer();

    for (size_t i = 0; i < context->num_iterations(); i++) {
        benchmark_insert(query, current_session);
    }

    context->stop_timer();

    mysql_teardown();

    benchmark_teardown();
});

BENCHMARK("postgres insert", [](benchpress::context *context) {

    auto uri_s = get_env_uri("POSTGRES_URI", "postgres://localhost/test");

    benchmark_setup(uri_s);

    postgres_setup();

    coda::db::insert_query query(current_session);

    query.into(user::TABLE_NAME).columns("first_name", "last_name", "dval");

    context->reset_timer();

    for (size_t i = 0; i < context->num_iterations(); i++) {
        benchmark_insert(query, current_session);
    }

    context->stop_timer();

    postgres_teardown();

    benchmark_teardown();
});

#ifndef HAVE_CONFIG_H
#include "config.h"
#endif
#include <benchpress/benchpress.hpp>
#include "benchmark.h"
#include "log.h"
#include "testicle.h"

using namespace arg3::db;

void benchmark_select(const std::string &tableName)
{
    select_query query(current_session);

    query.from(tableName);

    for (auto &row : query.execute()) {
        assert(row.column("first_name").is_valid());
    }
}

void benchmark_setup(const arg3::db::uri &uri_s)
{
    register_test_sessions();

    current_session = sqldb::create_session(uri_s);

    setup_current_session();
}

void benchmark_populate(benchpress::context *context)
{
    arg3::db::insert_query query(current_session);

    query.into(user::TABLE_NAME).columns("first_name", "last_name", "dval");

    for (size_t i = 0; i < context->num_iterations(); i++) {
        benchmark_insert(query, current_session);
    }
}

BENCHMARK("sqlite select", [](benchpress::context *context) {
    uri uri_s("file://test.db");

    benchmark_setup(uri_s);

    benchmark_populate(context);

    context->reset_timer();

    for (size_t i = 0; i < context->num_iterations(); i++) {
        benchmark_select(user::TABLE_NAME);
    }

    context->stop_timer();

    teardown_current_session();
});


// BENCHMARK("mysql select", [](benchpress::context* context)
// {
//      auto uri_s = get_env_uri("MYSQL_URI", "mysql://localhost/test");

//      benchmark_setup(uri_s);

//      benchmark_populate(context);

//      context->reset_timer();

//      for(size_t i = 0; i < context->num_iterations(); i++) {
//          benchmark_select(user::TABLE_NAME);
//      }

//      context->stop_timer();

//      teardown_current_session();
// });


// BENCHMARK("postgres select", [](benchpress::context* context)
// {
//      auto uri_s = get_env_uri("POSTGRES_URI", "postgres://localhost/test");

//      benchmark_setup(uri_s);

//      benchmark_populate(context);

//      context->reset_timer();

//      for(size_t i = 0; i < context->num_iterations(); i++) {
//          benchmark_select(user::TABLE_NAME);
//      }

//      context->stop_timer();

//      teardown_current_session();
// });

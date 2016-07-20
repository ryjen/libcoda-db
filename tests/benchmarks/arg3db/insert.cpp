#ifndef HAVE_CONFIG_H
#include "config.h"
#endif
#define BENCHPRESS_CONFIG_MAIN
#include <benchpress/benchpress.hpp>
#include "benchmark.h"
#include "db.test.h"
#include "log.h"
#include "testicle.h"

using namespace arg3::db;

void benchmark_insert(insert_query &insert, const std::shared_ptr<arg3::db::session> &session)
{
    insert.values(random_name(), random_name(), random_num<int>(-123012, 1231232));

    if (!insert.execute()) {
        arg3::db::log::error("unable to prepare test: %s", session->last_error().c_str());
    }
}

BENCHMARK("sqlite insert", [](benchpress::context *context) {
    uri uri_s("file://test.db");

    benchmark_setup(uri_s);

    arg3::db::insert_query query(current_session);

    query.into(user::TABLE_NAME).columns("first_name", "last_name", "dval");

    context->reset_timer();

    for (size_t i = 0; i < context->num_iterations(); i++) {
        benchmark_insert(query, current_session);
    }

    context->stop_timer();

    teardown_current_session();
});


// BENCHMARK("mysql insert", [](benchpress::context* context)
// {

//    auto uri_s = get_env_uri("MYSQL_URI", "mysql://localhost/test");

//    benchmark_setup(uri_s);

//    arg3::db::insert_query query(current_session);

//    query.into(user::TABLE_NAME).columns("first_name", "last_name", "dval");

//    context->reset_timer();

//    for(size_t i = 0; i < context->num_iterations(); i++) {
//        benchmark_insert(query, current_session);
//    }

//    context->stop_timer();

//    teardown_current_session();
// });

// BENCHMARK("postgres insert", [](benchpress::context* context)
// {

//    auto uri_s = get_env_uri("POSTGRES_URI", "postgres://localhost/test");

//    benchmark_setup(uri_s);

//    arg3::db::insert_query query(current_session);

//    query.into(user::TABLE_NAME).columns("first_name", "last_name", "dval");

//    context->reset_timer();

//    for(size_t i = 0; i < context->num_iterations(); i++) {
//        benchmark_insert(query, current_session);
//    }

//    context->stop_timer();

//    teardown_current_session();
// });

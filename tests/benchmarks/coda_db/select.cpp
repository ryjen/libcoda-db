
#include "benchmark.h"
#include "log.h"
#include "util.h"
#include <benchpress/benchpress.hpp>

using namespace coda::db;

BENCHMARK("sqlite select",[](
benchpress::context *context
) {
uri uri_s("file://test.db");

benchmark_setup(uri_s);

sqlite_setup();

benchmark_populate(context);

context->
reset_timer();

for (
size_t i = 0;
i<context->
num_iterations();
i++) {
benchmark_select(user::TABLE_NAME);
}

context->
stop_timer();

sqlite_teardown();

benchmark_teardown();
});

BENCHMARK("mysql select",[](
benchpress::context *context
) {
auto uri_s = get_env_uri("MYSQL_URI", "mysql://localhost/test");

benchmark_setup(uri_s);

mysql_setup();

benchmark_populate(context);

context->
reset_timer();

for (
size_t i = 0;
i<context->
num_iterations();
i++) {
benchmark_select(user::TABLE_NAME);
}

context->
stop_timer();

mysql_teardown();

benchmark_teardown();
});

BENCHMARK("postgres select",[](
benchpress::context *context
) {
auto uri_s = get_env_uri("POSTGRES_URI", "postgres://localhost/test");

benchmark_setup(uri_s);

postgres_setup();

benchmark_populate(context);

context->
reset_timer();

for (
size_t i = 0;
i<context->
num_iterations();
i++) {
benchmark_select(user::TABLE_NAME);
}

context->
stop_timer();

postgres_teardown();

benchmark_teardown();
});

#include "benchmark.h"
#include <benchpress/benchpress.hpp>
#include <soci/soci.h>
#include <unistd.h>

using namespace soci;

void benchmark_query(soci::session &session) {
  soci::rowset<soci::row> results =
      session.prepare << "select first_name, last_name, dval from users";

  for (auto &row : results) {
    row.get<std::string>("first_name");
  }
}

BENCHMARK("sqlite select", [](benchpress::context *context) {
  soci::session session("sqlite3", "test.db");

  create_table(session);

  for (size_t i = 0; i < context->num_iterations(); i++) {
    benchmark_insert(session);
  }

  context->reset_timer();

  for (size_t i = 0; i < context->num_iterations(); i++) {
    benchmark_query(session);
  }

  context->stop_timer();

  session.close();

  unlink("test.db");
});

BENCHMARK("mysql select", [](benchpress::context *context) {
  soci::session session("mysql", "dbname=test");

  create_mysql_table(session);

  for (size_t i = 0; i < context->num_iterations(); i++) {
    benchmark_insert(session);
  }

  context->reset_timer();

  for (size_t i = 0; i < context->num_iterations(); i++) {
    benchmark_query(session);
  }

  context->stop_timer();

  cleanup_table(session);

  session.close();
});

BENCHMARK("postgres select", [](benchpress::context *context) {
  soci::session session("postgresql", "dbname=test");

  create_postgres_table(session);

  for (size_t i = 0; i < context->num_iterations(); i++) {
    benchmark_insert(session);
  }

  context->reset_timer();

  for (size_t i = 0; i < context->num_iterations(); i++) {
    benchmark_query(session);
  }

  context->stop_timer();

  cleanup_table(session);

  session.close();
});

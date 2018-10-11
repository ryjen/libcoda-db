#include "benchmark.h"
#include <Poco/Data/SQLite/Connector.h>
#include <benchpress/benchpress.hpp>
#include <cassert>
#include <unistd.h>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

void benchmark_query(Session &session) {
  User user;

  Statement select(session);

  select << "SELECT first_name, last_name, dval FROM users",
      into(user.first_name), into(user.last_name), into(user.dval),
      range(0, 1); //  iterate over result set one row at a time

  while (!select.done()) {
    select.execute();
    assert(!user.first_name.empty());
  }
}

BENCHMARK("sqlite select", [](benchpress::context *context) {
  Poco::Data::SQLite::Connector::registerConnector();

  // create a session
  Session session("SQLite", "test.db");

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

// BENCHMARK("mysql select", [](benchpress::context* context) {
//     Poco::Data::MySQL::Connector::registerConnector();

//     // create a session
//     Session session("MySQL", "localhost/test");

//     create_table(session);

//     for (size_t i = 0; i < context->num_iterations(); i++) {
//         benchmark_insert(session);
//     }

//     context->reset_timer();

//     for (size_t i = 0; i < context->num_iterations(); i++) {
//         benchmark_query(session);
//     }

//     context->stop_timer();

//     session.close();
// });

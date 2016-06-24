#include <Poco/Data/Session.h>
#include <Poco/Data/SQLite/Connector.h>
#include <vector>
#include <iostream>
#include "random.test.h"
#define BENCHPRESS_CONFIG_MAIN
#include <benchpress/benchpress.hpp>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

struct User
{
    std::string first_name;
    std::string last_name;
    double      dval;
};

void benchmark_insert(Session &session)
{
    User user =
    {
        random_name(),
        random_name(),
        random_num<double>(-123012, 1231232)
    };

    Statement insert(session);
    insert << "INSERT INTO users(first_name, last_name, dval) VALUES(?, ?, ?)",
        use(user.first_name),
        use(user.last_name),
        use(user.dval);

    insert.execute();
}

void create_table(Session &session)
{
    // drop sample table, if it exists
    session << "create table if not exists users(id integer primary key autoincrement, first_name varchar(45), last_name varchar(45), dval real, data "
    "blob, tval timestamp)", now;
}

BENCHMARK("insert", [](benchpress::context *context) {
    // register SQLite connector
    Poco::Data::SQLite::Connector::registerConnector();

    // create a session
    Session session("SQLite", "test.db");

    create_table(session);

    context->reset_timer();

    for (size_t i = 0; i < context->num_iterations(); i++) {
        benchmark_insert(session);
    }
    context->stop_timer();
});

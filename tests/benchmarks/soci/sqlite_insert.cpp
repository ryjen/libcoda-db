#include <soci/soci.h>
#define BENCHPRESS_CONFIG_MAIN
#include <benchpress/benchpress.hpp>
#include "random.test.h"

using namespace soci;

struct User {
    std::string first_name;
    std::string last_name;
    double dval;
};

void benchmark_insert(soci::session &session)
{
    User user = { random_name(), random_name(), random_num<double>(-123012, 1231232) };

    session << "insert into users(first_name, last_name, dval) values(:first_name, :last_name, :dval)", use(user.first_name),
        use(user.last_name), use(user.dval);
}

BENCHMARK("insert", [](benchpress::context *context){

    soci::session session("sqlite3", "test.db");

    session << "create table if not exists users(id integer primary key autoincrement, first_name varchar(45), last_name varchar(45), dval real, data "
    "blob, tval timestamp)";

    context->reset_timer();

    for (size_t i = 0; i < context->num_iterations(); i++) {
        benchmark_insert(session);
    }

    context->stop_timer();
});

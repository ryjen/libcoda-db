#ifndef RJ_DB_BENCHMARK_H
#define RJ_DB_BENCHMARK_H

#include <Poco/Data/Session.h>

struct User {
    std::string first_name;
    std::string last_name;
    double dval;
};

extern void create_table(Poco::Data::Session &session);

extern void benchmark_insert(Poco::Data::Session &session);

#endif

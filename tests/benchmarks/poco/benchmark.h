#ifndef ARG3_DB_BENCHMARK_H
#define ARG3_DB_BENCHMARK_H

#include <Poco/Data/Session.h>

struct User {
    std::string first_name;
    std::string last_name;
    double dval;
};

extern void create_table(Poco::Data::Session &session);

extern void benchmark_insert(Poco::Data::Session &session);

#endif

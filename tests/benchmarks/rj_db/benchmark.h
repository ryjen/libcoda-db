#ifndef RJ_DB_BENCHMARK_H
#define RJ_DB_BENCHMARK_H

#include "db.test.h"

void benchmark_insert(rj::db::insert_query &insert, const std::shared_ptr<rj::db::session> &session);

void benchmark_select(const std::string &tableName);

void benchmark_setup(const rj::db::uri &uri_s);

void benchmark_populate(benchpress::context *context);

#endif

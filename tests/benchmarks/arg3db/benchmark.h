#ifndef ARG3_DB_BENCHMARK_H
#define ARG3_DB_BENCHMARK_H

void benchmark_insert(arg3::db::insert_query &insert, const std::shared_ptr<arg3::db::session> &session);

void benchmark_select(const std::string &tableName);

void benchmark_setup(const arg3::db::uri &uri_s);

void benchmark_populate(benchpress::context *context);

#endif

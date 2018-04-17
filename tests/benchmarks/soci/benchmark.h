#ifndef CODA_DB_BENCHMARK_H
#define CODA_DB_BENCHMARK_H

struct User {
    std::string first_name;
    std::string last_name;
    double dval;
};

void benchmark_insert(soci::session &session);
void benchmark_query(soci::session &session);
void create_table(soci::session &session);
void create_mysql_table(soci::session &session);
void create_postgres_table(soci::session &session);
void cleanup_table(soci::session &session);

#endif

#include "benchmark.h"
#include "log.h"
#include "mysql/session.h"
#include "postgres/session.h"
#include "sqlite/session.h"
#include "util.h"

using namespace coda::db;

std::shared_ptr<coda::db::session> current_session;

__attribute__((constructor)) void initialize(void) {
  coda::db::sqlite::initialize();
  coda::db::mysql::initialize();
  coda::db::postgres::initialize();
}

void sqlite_setup() {
  current_session->open();

  current_session->execute(
      "create table if not exists users(id integer primary key autoincrement, "
      "first_name varchar(45), last_name varchar(45), dval real, data "
      "blob, tval timestamp)");
  current_session->execute(
      "create table if not exists user_settings(id integer primary key "
      "autoincrement, user_id integer not null, valid int(1), created_at "
      "timestamp)");
}

void sqlite_teardown() {
  current_session->close();
  unlink(current_session->connection_info().path.c_str());
}

void mysql_setup() {
  current_session->open();
  current_session->execute(
      "create table if not exists users(id integer primary key auto_increment, "
      "first_name varchar(45), last_name varchar(45), dval real, data "
      "blob, tval timestamp)");
  current_session->execute(
      "create table if not exists user_settings(id integer primary key "
      "auto_increment, user_id integer not null, valid int(1), created_at "
      "timestamp)");
}

void mysql_teardown() {
  current_session->execute("drop table users");
  current_session->execute("drop table user_settings");
  current_session->close();
}

void postgres_setup() {
  current_session->open();
  current_session->execute(
      "create table if not exists users(id serial primary key unique, "
      "first_name varchar(45), last_name varchar(45), dval real, data bytea, "
      "tval "
      "timestamp)");
  current_session->execute(
      "create table if not exists user_settings(id serial primary key unique, "
      "user_id integer not null, valid smallint, created_at timestamp)");
}

void postgres_teardown() {
  current_session->execute("drop table users");
  current_session->execute("drop table user_settings");
  current_session->close();
}

void benchmark_select(const std::string &tableName) {
  select_query query(current_session);

  query.from(tableName);

  for (auto &row : query.execute()) {
    assert(row.column("first_name").is_valid());
  }
}

void benchmark_setup(const coda::db::uri &uri_s) {
  current_session = sqldb::create_session(uri_s);

  current_session->open();
}

void benchmark_teardown() { current_session->close(); }

void benchmark_insert(insert_query &insert,
                      const std::shared_ptr<coda::db::session> &session) {
  insert.values(random_name(), random_name(),
                random_num<int>(-123012, 1231232));

  if (!insert.execute()) {
    coda::db::log::error("unable to prepare test: %s",
                         session->last_error().c_str());
  }
}

void benchmark_populate(benchpress::context *context) {
  coda::db::insert_query query(current_session);

  query.into(user::TABLE_NAME).columns("first_name", "last_name", "dval");

  for (size_t i = 0; i < context->num_iterations(); i++) {
    benchmark_insert(query, current_session);
  }
}

#include <string>

#include "../db.test.h"

#include "postgres/column.h"
#include <bandit/bandit.h>
#include <libpq-fe.h>
#include <memory>
#include <postgres.h>
#include <catalog/pg_type.h>

using namespace bandit;

using namespace std;

using namespace coda::db;

using namespace snowhouse;

shared_ptr<postgres::column> get_postgres_column(const string &name) {
  select_query q(test::current_session, {}, "users");

  auto rs = q.execute();

  auto row = rs.begin();

  auto col = row->column(name);

  return static_pointer_cast<postgres::column>(col.impl());
}

SPEC_BEGIN(postgres_column) {
  describe("postgres column", []() {
    before_each([]() {
      test::setup_current_session();

      test::user user1;

      user1.set("first_name", "test");
      user1.set("last_name", "test");

      user1.save();
    });

    after_each([]() { test::teardown_current_session(); });

    it("is movable", []() {
      auto col = get_postgres_column("first_name");

      postgres::column &&other(std::move(*col));

      AssertThat(other.is_valid(), IsTrue());

      auto c2 = get_postgres_column("last_name");

      postgres::column last = std::move(*c2);

      last = std::move(other);

      AssertThat(last.is_valid(), IsTrue());
    });
    it("has a type", []() {
      auto col = get_postgres_column("first_name");

      Assert::That(col->sql_type(), Equals(VARCHAROID));
    });

    it("has a name", []() {
      auto col = get_postgres_column("last_name");

      Assert::That(col->name(), Equals("last_name"));
    });
  });
}
SPEC_END;

#include <string>

#include "resultset.h"
#include "postgres/resultset.h"

#include "session.h"
#include "postgres/session.h"

#include "../db.test.h"

#include <bandit/bandit.h>

using namespace bandit;

using namespace std;

using namespace coda::db;

using namespace snowhouse;

shared_ptr<postgres::resultset> get_postgres_resultset() {
  select_query query(test::current_session, {}, test::user::TABLE_NAME);

  auto rs = query.execute();

  return static_pointer_cast<postgres::resultset>(rs.impl());
}

SPEC_BEGIN(postgres_resultset) {
  describe("a postgres resultset", []() {
    before_each([]() {
      test::setup_current_session();

      test::user user1;

      user1.set("first_name", "Bryan");
      user1.set("last_name", "Jenkins");

      user1.save();

      test::user user2;

      user2.set("first_name", "Bob");
      user2.set("last_name", "Smith");

      user2.set("dval", 3.1456);

      user2.save();
    });

    after_each([]() { test::teardown_current_session(); });

    it("requires valid initializer parameters", []() {
      AssertThrows(database_exception, postgres::resultset(nullptr, nullptr));
      AssertThrows(database_exception,
                   postgres::resultset(dynamic_pointer_cast<postgres::session>(
                                           test::current_session->impl()),
                                       nullptr));
    });

    it("is movable", []() {
      auto rs = get_postgres_resultset();

      postgres::resultset other(std::move(*rs));

      Assert::That(other.is_valid(), IsTrue());

      rs = get_postgres_resultset();

      other = std::move(*rs);

      Assert::That(other.is_valid(), IsTrue());
    });
  });
}
SPEC_END;

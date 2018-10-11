#include <string>

#include "../db.test.h"
#include "sqlite/row.h"
#include "sqlite/session.h"
#include <bandit/bandit.h>

using namespace bandit;

using namespace std;

using namespace coda::db;

using namespace snowhouse;

template <typename T> shared_ptr<T> get_sqlite_row(size_t index) {
  select_query query(test::current_session, {}, test::user::TABLE_NAME);

  auto rs = query.execute();

  if (index > 0 && index >= rs.size()) {
    throw database_exception("not enough rows");
  }

  auto i = rs.begin();

  if (index > 0) {
    i += index;
  }

  if (i == rs.end()) {
    throw database_exception("no rows found");
  }

  return dynamic_pointer_cast<T>(i->impl());
}

SPEC_BEGIN(sqlite_row) {
  describe("a sqlite row", []() {
    auto sqlite_session =
        dynamic_pointer_cast<sqlite::session>(test::current_session->impl());

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

    it("requires initializer parameters", [&sqlite_session]() {
      AssertThrows(database_exception, sqlite::row(nullptr, nullptr));

      AssertThrows(database_exception, sqlite::row(sqlite_session, nullptr));
    });

    it("is movable", [&sqlite_session]() {
      auto row = get_sqlite_row<sqlite::row>(0);

      Assert::That(row != nullptr, IsTrue());

      sqlite::row other(std::move(*row));

      Assert::That(other.is_valid(), IsTrue());

      row = get_sqlite_row<sqlite::row>(0);

      other = std::move(*row);

      Assert::That(other.is_valid(), IsTrue());
    });

    it("can handle invalid column selection", [&sqlite_session]() {
      auto row = get_sqlite_row<sqlite::row>(0);

      Assert::That(row != nullptr, IsTrue());

      AssertThrows(no_such_column_exception, row->column(1234));

      AssertThrows(no_such_column_exception, row->column(""));

      AssertThrows(no_such_column_exception, row->column("abracadabra"));

      AssertThrows(no_such_column_exception, row->column_name(1234));
    });
  });
}
SPEC_END;

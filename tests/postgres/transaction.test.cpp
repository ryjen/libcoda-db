#include <string>

#include "../db.test.h"
#include "insert_query.h"
#include "postgres/session.h"
#include "postgres/transaction.h"
#include <bandit/bandit.h>

using namespace bandit;

using namespace std;

using namespace coda::db;

using namespace snowhouse;

SPEC_BEGIN(postgres_transaction) {
  describe("postgres transaction", []() {
    before_each([]() { test::setup_current_session(); });

    after_each([]() { test::teardown_current_session(); });

    auto pg_session =
        dynamic_pointer_cast<postgres::session>(test::current_session->impl());

    describe("isolation level", [&pg_session]() {
      it("can be serializable", [&pg_session]() {
        postgres::transaction::mode mode{transaction::isolation::serializable};

        auto pg_tx = pg_session->create_transaction(mode);

        transaction tx(test::current_session, pg_tx);

        tx.start();

        insert_query insert(test::current_session);

        insert.into(test::user::TABLE_NAME)
            .columns("first_name", "last_name")
            .values("Sam", "Baggins");

        Assert::That(insert.execute(), IsTrue());

        tx.commit();
      });

      it("can be repeatable read", [&pg_session]() {
        postgres::transaction::mode mode{
            transaction::isolation::repeatable_read};

        auto pg_tx = pg_session->create_transaction(mode);

        transaction tx(test::current_session, pg_tx);

        tx.start();

        insert_query insert(test::current_session);

        insert.into(test::user::TABLE_NAME)
            .columns("first_name", "last_name")
            .values("Sam", "Baggins");

        Assert::That(insert.execute(), IsTrue());

        tx.commit();
      });

      it("can be read commited", [&pg_session]() {
        postgres::transaction::mode mode{transaction::isolation::read_commited};

        auto pg_tx = pg_session->create_transaction(mode);

        transaction tx(test::current_session, pg_tx);

        tx.start();

        insert_query insert(test::current_session);

        insert.into(test::user::TABLE_NAME)
            .columns("first_name", "last_name")
            .values("Sam", "Baggins");

        Assert::That(insert.execute(), IsTrue());

        tx.commit();
      });

      it("can be read uncommited", [&pg_session]() {
        postgres::transaction::mode mode{
            transaction::isolation::read_uncommited};

        auto pg_tx = pg_session->create_transaction(mode);

        transaction tx(test::current_session, pg_tx);

        tx.start();

        insert_query insert(test::current_session);

        insert.into(test::user::TABLE_NAME)
            .columns("first_name", "last_name")
            .values("Sam", "Baggins");

        Assert::That(insert.execute(), IsTrue());

        tx.commit();
      });
    });

    describe("transaction type", [&pg_session]() {
      it("can be read write", [&pg_session]() {
        postgres::transaction::mode mode{transaction::isolation::serializable,
                                         transaction::read_write};

        auto pg_tx = pg_session->create_transaction(mode);

        transaction tx(test::current_session, pg_tx);

        tx.start();

        insert_query insert(test::current_session);

        insert.into(test::user::TABLE_NAME)
            .columns("first_name", "last_name")
            .values("Sam", "Baggins");

        Assert::That(insert.execute(), IsTrue());

        tx.commit();
      });
      it("can be read only", [&pg_session]() {
        postgres::transaction::mode mode{transaction::isolation::serializable,
                                         transaction::read_only};

        auto pg_tx = pg_session->create_transaction(mode);

        transaction tx(test::current_session, pg_tx);

        tx.start();

        insert_query insert(test::current_session);

        insert.into(test::user::TABLE_NAME)
            .columns("first_name", "last_name")
            .values("Sam", "Baggins");

        Assert::That(insert.execute(), IsFalse());

        tx.commit();
      });
    });
    it("can be deferrable", [&pg_session]() {
      postgres::transaction::mode mode{transaction::isolation::serializable,
                                       transaction::read_write, true};

      auto pg_tx = pg_session->create_transaction(mode);

      transaction tx(test::current_session, pg_tx);

      tx.start();

      insert_query insert(test::current_session);

      insert.into(test::user::TABLE_NAME)
          .columns("first_name", "last_name")
          .values("Sam", "Baggins");

      Assert::That(insert.execute(), IsTrue());

      tx.commit();
    });
  });
}
SPEC_END;

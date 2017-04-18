#include <string>

#include <bandit/bandit.h>
#include "../db.test.h"
#include "insert_query.h"
#include "sqlite/session.h"
#include "sqlite/transaction.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

using namespace snowhouse;

SPEC_BEGIN(sqlite_transaction)
{
    describe("sqlite transaction", []() {
        before_each([]() { test::setup_current_session(); });

        after_each([]() { test::teardown_current_session(); });

        auto sqlite_session = dynamic_pointer_cast<sqlite::session>(test::current_session->impl());

        describe("type", [&sqlite_session]() {

            it("can be deferred", [&sqlite_session]() {
                sqlite::transaction::type mode = sqlite::transaction::deferred;

                auto sqlite_tx = sqlite_session->create_transaction(mode);

                transaction tx(test::current_session, sqlite_tx);

                tx.start();

                insert_query insert(test::current_session);

                insert.into(test::user::TABLE_NAME).columns("first_name", "last_name").values("Sam", "Baggins");

                Assert::That(insert.execute(), IsTrue());

                tx.commit();
            });

            it("can be immediate", [&sqlite_session]() {
                sqlite::transaction::type mode = sqlite::transaction::immediate;

                auto sqlite_tx = sqlite_session->create_transaction(mode);

                transaction tx(test::current_session, sqlite_tx);

                tx.start();

                insert_query insert(test::current_session);

                insert.into(test::user::TABLE_NAME).columns("first_name", "last_name").values("Sam", "Baggins");

                Assert::That(insert.execute(), IsTrue());

                tx.commit();
            });

            it("can be exclusive", [&sqlite_session]() {
                sqlite::transaction::type mode = sqlite::transaction::exclusive;

                auto sqlite_tx = sqlite_session->create_transaction(mode);

                transaction tx(test::current_session, sqlite_tx);

                tx.start();

                insert_query insert(test::current_session);

                insert.into(test::user::TABLE_NAME).columns("first_name", "last_name").values("Sam", "Baggins");

                Assert::That(insert.execute(), IsTrue());

                tx.commit();
            });

            it("can error", [&sqlite_session]() {
                sqlite::transaction::type mode = sqlite::transaction::exclusive;

                auto sqlite_tx = sqlite_session->create_transaction(mode);

                transaction tx(test::current_session, sqlite_tx);

                tx.start();

                tx.commit();

                AssertThrows(transaction_exception, tx.rollback());

                AssertThrows(transaction_exception, tx.commit());
            });
        });

    });
}
SPEC_END;

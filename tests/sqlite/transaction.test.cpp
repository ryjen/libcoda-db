
#include <bandit/bandit.h>
#include "../db.test.h"
#include "sqlite/transaction.h"

#ifdef HAVE_LIBSQLITE3

using namespace bandit;

using namespace std;

using namespace arg3::db;

go_bandit([]() {

    describe("sqlite transaction", []() {
        before_each([]() { setup_current_session(); });

        after_each([]() { teardown_current_session(); });

        auto sqlite_session = dynamic_pointer_cast<sqlite::session>(current_session->impl());

        describe("type", [&sqlite_session]() {

            it("can be deferred", [&sqlite_session]() {
                sqlite::transaction::type mode = sqlite::transaction::deferred;

                auto sqlite_tx = sqlite_session->create_transaction(mode);

                transaction tx(current_session, sqlite_tx);

                tx.start();

                insert_query insert(current_session);

                insert.into("users").columns("first_name", "last_name").values("Sam", "Baggins");

                Assert::That(insert.execute(), IsTrue());

                tx.commit();
            });

            it("can be immediate", [&sqlite_session]() {
                sqlite::transaction::type mode = sqlite::transaction::immediate;

                auto sqlite_tx = sqlite_session->create_transaction(mode);

                transaction tx(current_session, sqlite_tx);

                tx.start();

                insert_query insert(current_session);

                insert.into("users").columns("first_name", "last_name").values("Sam", "Baggins");

                Assert::That(insert.execute(), IsTrue());

                tx.commit();
            });

            it("can be exclusive", [&sqlite_session]() {
                sqlite::transaction::type mode = sqlite::transaction::exclusive;

                auto sqlite_tx = sqlite_session->create_transaction(mode);

                transaction tx(current_session, sqlite_tx);

                tx.start();

                insert_query insert(current_session);

                insert.into("users").columns("first_name", "last_name").values("Sam", "Baggins");

                Assert::That(insert.execute(), IsTrue());

                tx.commit();
            });

            it("can error", [&sqlite_session]() {
                sqlite::transaction::type mode = sqlite::transaction::exclusive;

                auto sqlite_tx = sqlite_session->create_transaction(mode);

                transaction tx(current_session, sqlite_tx);

                tx.start();

                tx.commit();

                AssertThrows(transaction_exception, tx.rollback());

                AssertThrows(transaction_exception, tx.commit());
            });
        });

    });

});

#endif

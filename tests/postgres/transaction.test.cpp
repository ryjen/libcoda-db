
#include <bandit/bandit.h>
#include "../db.test.h"
#include "postgres/transaction.h"

#ifdef HAVE_LIBPQ

using namespace bandit;

using namespace std;

using namespace arg3::db;

go_bandit([]() {

    describe("postgres transaction", []() {
        before_each([]() { setup_current_session(); });

        after_each([]() { teardown_current_session(); });

        auto pg_session = dynamic_pointer_cast<postgres::session>(current_session->impl());

        describe("isolation level", [&pg_session]() {

            it("can be serializable", [&pg_session]() {
                postgres::transaction::mode mode{isolation::serializable};

                auto pg_tx = pg_session->create_transaction(mode);

                transaction tx(current_session, pg_tx);

                tx.start();

                insert_query insert(current_session);

                insert.into("users").columns("first_name", "last_name").values("Sam", "Baggins");

                Assert::That(insert.execute(), IsTrue());

                tx.commit();
            });

            it("can be repeatable read", [&pg_session]() {
                postgres::transaction::mode mode{isolation::repeatable_read};

                auto pg_tx = pg_session->create_transaction(mode);

                transaction tx(current_session, pg_tx);

                tx.start();

                insert_query insert(current_session);

                insert.into("users").columns("first_name", "last_name").values("Sam", "Baggins");

                Assert::That(insert.execute(), IsTrue());

                tx.commit();
            });

            it("can be read commited", [&pg_session]() {
                postgres::transaction::mode mode{isolation::read_commited};

                auto pg_tx = pg_session->create_transaction(mode);

                transaction tx(current_session, pg_tx);

                tx.start();

                insert_query insert(current_session);

                insert.into("users").columns("first_name", "last_name").values("Sam", "Baggins");

                Assert::That(insert.execute(), IsTrue());

                tx.commit();
            });

            it("can be read uncommited", [&pg_session]() {
                postgres::transaction::mode mode{isolation::read_uncommited};

                auto pg_tx = pg_session->create_transaction(mode);

                transaction tx(current_session, pg_tx);

                tx.start();

                insert_query insert(current_session);

                insert.into("users").columns("first_name", "last_name").values("Sam", "Baggins");

                Assert::That(insert.execute(), IsTrue());

                tx.commit();
            });
        });

        describe("transaction type", [&pg_session]() {
            it("can be read write", [&pg_session]() {
                postgres::transaction::mode mode{isolation::serializable, transaction::read_write};

                auto pg_tx = pg_session->create_transaction(mode);

                transaction tx(current_session, pg_tx);

                tx.start();

                insert_query insert(current_session);

                insert.into("users").columns("first_name", "last_name").values("Sam", "Baggins");

                Assert::That(insert.execute(), IsTrue());

                tx.commit();
            });
            it("can be read only", [&pg_session]() {
                postgres::transaction::mode mode{isolation::serializable, transaction::read_only};

                auto pg_tx = pg_session->create_transaction(mode);

                transaction tx(current_session, pg_tx);

                tx.start();

                insert_query insert(current_session);

                insert.into("users").columns("first_name", "last_name").values("Sam", "Baggins");

                Assert::That(insert.execute(), IsFalse());

                tx.commit();
            });
        });
        it("can be deferrable", [&pg_session]() {
            postgres::transaction::mode mode{isolation::serializable, transaction::read_write, true};

            auto pg_tx = pg_session->create_transaction(mode);

            transaction tx(current_session, pg_tx);

            tx.start();

            insert_query insert(current_session);

            insert.into("users").columns("first_name", "last_name").values("Sam", "Baggins");

            Assert::That(insert.execute(), IsTrue());

            tx.commit();
        });
    });

});

#endif

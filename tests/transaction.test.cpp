#include <bandit/bandit.h>
#include "db.test.h"
#include "transaction.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

specification(transactions, []() {
    describe("a transaction", []() {

        before_each([]() {
            test::user user1;
            test::user user2;

            try {
                test::setup_current_session();

                user1.set("first_name", "Bryan");
                user1.set("last_name", "Jenkins");

                user1.save();

                user2.set("first_name", "Mark");
                user2.set("last_name", "Smith");

                user2.save();

            } catch (const std::exception& e) {
                cerr << "setup error: " << e.what() << endl;
                throw e;
            }
        });

        after_each([]() { test::teardown_current_session(); });

        it("can copy", []() {
            auto tx = test::current_session->create_transaction();

            tx.start();

            Assert::That(tx.is_active(), IsTrue());

            auto other(tx);

            Assert::That(other.is_active(), IsTrue());

            transaction assign(test::current_session, other.impl());

            assign = tx;

            Assert::That(assign.is_active(), IsTrue());
        });

        it("is can be moved", []() {
            auto tx = test::current_session->create_transaction();

            tx.set_successful(true);

            auto other(std::move(tx));

            Assert::That(other.is_successful(), Equals(true));

            transaction assign(test::current_session, other.impl());

            assign = std::move(tx);

            Assert::That(other.is_successful(), Equals(true));
        });

        it("will commit on scope loss", []() {

            auto other_session = sqldb::open_session(test::current_session->connection_info());

            select_query select(other_session);

            select.from(test::user::TABLE_NAME).where(op::equals("first_name", "Mike")) && op::equals("last_name", "Johnson");

            {
                auto tx = test::current_session->start_transaction();

                insert_query insert(tx.get_session());

                insert.into(test::user::TABLE_NAME).columns("first_name", "last_name").values("Mike", "Johnson");

                Assert::That(insert.execute(), IsTrue());

                auto rs = select.execute();

                Assert::That(rs.size(), Equals(0));

                tx.set_successful(true);
            }

            auto rs = select.execute();

            Assert::That(rs.size(), Equals(1));
        });

        it("can commit a transaction", []() {
            // create the transaction in the current session
            transaction trans = test::current_session->create_transaction();

            // open a second session
            auto other_session = sqldb::open_session(test::current_session->connection_info());

            // do some transaction work
            trans.start();

            Assert::That(other_session->is_open(), IsTrue());

            insert_query insert(trans.get_session());

            insert.into(test::user::TABLE_NAME).columns({"first_name", "last_name"}).values("Jerome", "Padington");

            Assert::That(insert.execute(), IsTrue());

            // now try to read the work before its committed
            select_query select(other_session);

            select.from(test::user::TABLE_NAME).where(op::equals("first_name", "Jerome")) && op::equals("last_name", "Padington");

            auto results = select.execute();

            auto it = results.begin();

            Assert::That(results.size(), Equals(0));

            // now commit and make sure we can read it
            trans.commit();

            results = select.execute();

            Assert::That(results.size(), Equals(1));
        });
        it("can rollback a transaction", []() {
            // create the transaction in the current session
            transaction trans = test::current_session->create_transaction();

            // open a second session
            auto other_session = sqldb::open_session(test::current_session->connection_info());

            // do some transaction work
            trans.start();

            Assert::That(other_session->is_open(), IsTrue());

            insert_query insert(trans.get_session());

            insert.into(test::user::TABLE_NAME).columns({"first_name", "last_name"}).values("Jerome", "Padington");

            Assert::That(insert.execute(), IsTrue());

            // now try to read the work before its committed
            select_query select(other_session);

            select.from(test::user::TABLE_NAME).where(op::equals("first_name", "Jerome")) && op::equals("last_name", "Padington");

            auto results = select.execute();

            auto it = results.begin();

            Assert::That(results.size(), Equals(0));

            // now commit and make sure we can read it
            trans.rollback();

            results = select.execute();

            Assert::That(results.size(), Equals(0));
        });
        it("can set a savepoint", []() {
            auto tx = test::current_session->create_transaction();

            auto other_session = sqldb::open_session(test::current_session->connection_info());

            tx.start();

            insert_query insert(tx.get_session());

            insert.into(test::user::TABLE_NAME).columns({"first_name", "last_name"}).values("Jerome", "Padington");

            Assert::That(insert.execute(), IsTrue());

            tx.save("padington");

            Assert::That(insert.execute(), IsTrue());

            tx.rollback("padington");

            Assert::That(insert.execute(), IsTrue());

            select_query select(test::current_session);

            select.from(test::user::TABLE_NAME).where(op::equals("last_name", "Padington"));

            auto rs = select.execute();

            Assert::That(rs.size(), Equals(2));
        });

        it("can release a savepoint", []() {
            auto tx = test::current_session->create_transaction();

            tx.start();

            insert_query insert(tx.get_session());

            insert.into(test::user::TABLE_NAME).columns({"first_name", "last_name"}).values("Jerome", "Padington");

            Assert::That(insert.execute(), IsTrue());

            tx.save("padington");

            Assert::That(insert.execute(), IsTrue());

            tx.release("padington");

            AssertThrows(transaction_exception, tx.release("padington"));
        });
    });
});

#include <bandit/bandit.h>
#include "transaction.h"
#include "db.test.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

go_bandit([]() {

    describe("a transaction", []() {

        before_each([]() {
            user user1;
            user user2;

            try {
                setup_current_session();

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

        after_each([]() { teardown_current_session(); });

        it("can commit a transaction", []() {
            transaction trans = current_session->create_transaction();

            auto other_session = sqldb::create_session(current_session->connection_info());

            other_session->open();

            trans.start();

            Assert::That(other_session->is_open(), IsTrue());

            insert_query insert(trans.session());

            insert.into("users").columns({"first_name", "last_name"}).values("Jerome", "Padington");

            Assert::That(insert.execute(), IsTrue());

            select_query select(other_session);

            select.from("users").where("first_name = $1 and last_name = $2", "Jerome", "Padington");

            auto results = select.execute();

            auto it = results.begin();

            Assert::That(results.size(), Equals(0));

            trans.commit();

            results = select.execute();

            Assert::That(results.size(), Equals(1));
        });
    });
});

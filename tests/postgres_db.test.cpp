#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#undef VERSION

#if defined(HAVE_LIBPQ) && defined(TEST_POSTGRES)

#include <bandit/bandit.h>
#include "db.test.h"
#include "postgres/db.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;


go_bandit([]() {

    describe("postgres database", []() {
        before_each([]() { postgres_testdb.setup(); });
        after_each([]() { postgres_testdb.teardown(); });

        it("is copyable", []() {
            postgres_db db(postgres_testdb);

            Assert::That(db.is_open(), IsTrue());

            Assert::That(db.connection_info().value, Equals(postgres_testdb.connection_info().value));

            Assert::That(postgres_testdb.is_open(), IsTrue());

            db = postgres_testdb;

            Assert::That(db.connection_info().value, Equals(postgres_testdb.connection_info().value));

            Assert::That(db.is_open(), IsTrue());

            Assert::That(postgres_testdb.is_open(), IsTrue());

            db.open();

            Assert::That(db.is_open(), IsTrue());

        });

        it("is movable", []() {
            postgres_db other(postgres_testdb);

            other.open();

            Assert::That(other.is_open(), IsTrue());

            postgres_db db(std::move(other));

            Assert::That(other.is_open(), IsFalse());

            Assert::That(db.is_open(), IsTrue());

            other = std::move(db);

            Assert::That(db.is_open(), IsFalse());

            Assert::That(other.is_open(), IsTrue());

            other.close();
        });

        it("can handle bad parameters", []() {
            postgres_db db(uri("postgres://zzzzz:zzzzz@zzzz/zzzzz:0"));

            AssertThrows(database_exception, db.open());
        });
    });

});

#endif

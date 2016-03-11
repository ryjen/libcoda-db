
#include <bandit/bandit.h>
#include "db.test.h"
#include "sqlite/statement.h"

#if defined(HAVE_LIBSQLITE3) && defined(TEST_SQLITE)

using namespace bandit;

using namespace std;

using namespace arg3::db;

go_bandit([]() {

    describe("sqlite3 statement", []() {
        auto sqlite_session = dynamic_pointer_cast<sqlite::session>(current_session->impl());

        before_each([]() { setup_current_session(); });

        after_each([]() { teardown_current_session(); });

        it("is movable", [&sqlite_session]() {
            sqlite::statement stmt(sqlite_session);

            stmt.prepare("select * from users");

            AssertThat(stmt.is_valid(), IsTrue());

            sqlite::statement s2(std::move(stmt));

            AssertThat(s2.is_valid(), IsTrue());

            AssertThat(stmt.is_valid(), IsFalse());

            sqlite::statement s3(sqlite_session);

            AssertThat(s3.is_valid(), IsFalse());

            s3 = std::move(s2);

            AssertThat(s3.is_valid(), IsTrue());

            AssertThat(s2.is_valid(), IsFalse());
        });

        it("throws exceptions", [&sqlite_session]() {
            auto session = sqldb::create_session("file");

            sqlite::statement stmt(dynamic_pointer_cast<arg3::db::sqlite::session>(session->impl()));

            AssertThrows(database_exception, stmt.prepare("select * from users"));

            stmt = sqlite::statement(sqlite_session);

            AssertThrows(database_exception, stmt.prepare("asdfasdfasdf"));

            AssertThat(stmt.last_error().empty(), IsFalse());

            AssertThrows(binding_error, stmt.bind(1, 1));

            AssertThrows(binding_error, stmt.bind(1, 1234LL));

            AssertThrows(binding_error, stmt.bind(1, 123.123));

            AssertThrows(binding_error, stmt.bind(1, "12134123"));

            AssertThrows(binding_error, stmt.bind(1, sql_blob()));

            AssertThrows(binding_error, stmt.bind(1, sql_null));
        });

        it("can reset", [&sqlite_session]() {
            sqlite::statement stmt(sqlite_session);

            stmt.prepare("select * from users");

            AssertThat(stmt.is_valid(), IsTrue());

            stmt.reset();

            stmt.prepare("select id, first_name from users where id > 0");

            AssertThat(stmt.is_valid(), IsTrue());
        });

    });

});

#endif

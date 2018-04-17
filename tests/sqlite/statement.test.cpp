#include <string>

#include <bandit/bandit.h>
#include <climits>
#include "../db.test.h"
#include "sqlite/session.h"
#include "sqlite/statement.h"

using namespace bandit;

using namespace std;

using namespace coda::db;

using namespace snowhouse;

SPEC_BEGIN(sqlite_statement)
{
    describe("sqlite3 statement", []() {
        auto sqlite_session = dynamic_pointer_cast<sqlite::session>(test::current_session->impl());

        before_each([]() { test::setup_current_session(); });

        after_each([]() { test::teardown_current_session(); });

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

            sqlite::statement stmt(dynamic_pointer_cast<sqlite::session>(session->impl()));

            AssertThrows(database_exception, stmt.prepare("select * from users"));

            stmt = sqlite::statement(sqlite_session);

            AssertThrows(database_exception, stmt.prepare("asdfasdfasdf"));

            AssertThat(stmt.last_error().empty(), IsFalse());

            AssertThat(stmt.result(), IsFalse());
        });

        it("can handle bad bindings", [&sqlite_session]() {

            sqlite::statement stmt(sqlite_session);

            AssertThrows(binding_error, stmt.bind(INT_MAX, 12341234));

            AssertThrows(binding_error, stmt.bind(INT_MAX, 1234U));

            AssertThrows(binding_error, stmt.bind(INT_MAX, 1234LL));

            AssertThrows(binding_error, stmt.bind(INT_MAX, 1234ULL));

            AssertThrows(binding_error, stmt.bind(INT_MAX, 123.123));
            AssertThrows(binding_error, stmt.bind(INT_MAX, 123.123f));

            AssertThrows(binding_error, stmt.bind(INT_MAX, "12134123"));
            AssertThrows(binding_error, stmt.bind(INT_MAX, L"12134123"));

            AssertThrows(binding_error, stmt.bind(INT_MAX, sql_blob()));
            AssertThrows(binding_error, stmt.bind(INT_MAX, sql_time()));

            AssertThrows(binding_error, stmt.bind(INT_MAX, sql_null));
            AssertThrows(binding_error, stmt.bind("blah", sql_value()));
        });


        it("can handle bad bindings with prepared sql", [&sqlite_session]() {

            sqlite::statement stmt(sqlite_session);

            stmt.prepare("select * from users");

            AssertThrows(binding_error, stmt.bind(INT_MAX, 12341234));

            AssertThrows(binding_error, stmt.bind(INT_MAX, 1234U));

            AssertThrows(binding_error, stmt.bind(INT_MAX, 1234LL));

            AssertThrows(binding_error, stmt.bind(INT_MAX, 1234ULL));

            AssertThrows(binding_error, stmt.bind(INT_MAX, 123.123));

            AssertThrows(binding_error, stmt.bind(INT_MAX, 123.123f));

            AssertThrows(binding_error, stmt.bind(INT_MAX, "12134123"));
            AssertThrows(binding_error, stmt.bind(INT_MAX, L"12134123"));

            AssertThrows(binding_error, stmt.bind(INT_MAX, sql_blob()));
            AssertThrows(binding_error, stmt.bind(INT_MAX, sql_time()));

            AssertThrows(binding_error, stmt.bind(INT_MAX, sql_null));
            AssertThrows(binding_error, stmt.bind("@blah", sql_value()));
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
}
SPEC_END;

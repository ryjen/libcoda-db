
#include <bandit/bandit.h>
#include <memory>
#include "../db.test.h"
#include "sqlite/column.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

template <typename T>
shared_ptr<T> get_sqlite_column(const string &name)
{
    select_query q(current_session);

    auto rs = q.from("users").execute();

    auto row = rs.begin();

    auto col = row->column(name);

    return static_pointer_cast<T>(col.impl());
}


go_bandit([]() {

    describe("sqlite column", []() {
        before_each([]() {
            setup_current_session();

            user user1;

            user1.set("first_name", "test");
            user1.set("last_name", "test");

            user1.save();

        });

        after_each([]() { teardown_current_session(); });

        auto sqlite_session = dynamic_pointer_cast<sqlite::session>(current_session->impl());

        it("is movable", [&sqlite_session]() {
            auto col = get_sqlite_column<sqlite::column>("first_name");

            sqlite::column other(std::move(*col));

            Assert::That(other.is_valid(), IsTrue());
            Assert::That(col->is_valid(), IsFalse());

            col = get_sqlite_column<sqlite::column>("last_name");

            other = std::move(*col);

            Assert::That(other.is_valid(), IsTrue());
            Assert::That(col->is_valid(), IsFalse());
        });

        it("has a type", [&sqlite_session]() {

            auto col = get_sqlite_column<sqlite::column>("first_name");

            Assert::That(col->sql_type(), Equals(SQLITE_TEXT));
        });

        it("has a name", [&sqlite_session]() {

            auto col = get_sqlite_column<sqlite::column>("last_name");

            Assert::That(col->name(), Equals("last_name"));
        });
    });
});

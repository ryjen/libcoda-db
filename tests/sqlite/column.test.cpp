#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(HAVE_LIBSQLITE3) && defined(TEST_SQLITE)

#include <bandit/bandit.h>
#include <memory>
#include "../db.test.h"
#include "sqlite/column.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

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

        describe("has a type", [&sqlite_session]() {

            it("as a column", [&sqlite_session]() {

                sqlite_session->cache_level(sqlite::cache::None);

                auto col = get_sqlite_column<sqlite::column>("first_name");

                Assert::That(col->sql_type(), Equals(SQLITE_TEXT));
            });

            it("as a cached column", [&sqlite_session]() {
                sqlite_session->cache_level(sqlite::cache::ResultSet);

                auto col = get_sqlite_column<sqlite::cached_column>("first_name");

                Assert::That(col->sql_type(), Equals(SQLITE_TEXT));
            });
        });

        describe("has a name", [&sqlite_session]() {

            it("as a column", [&sqlite_session]() {

                sqlite_session->cache_level(sqlite::cache::None);

                auto col = get_sqlite_column<sqlite::column>("last_name");

                Assert::That(col->name(), Equals("last_name"));
            });

            it("as a cached column", [&sqlite_session]() {

                sqlite_session->cache_level(sqlite::cache::ResultSet);

                auto col = get_sqlite_column<sqlite::cached_column>("last_name");

                Assert::That(col->name(), Equals("last_name"));
            });
        });
    });
});

#endif

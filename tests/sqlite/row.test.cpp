
#include <bandit/bandit.h>
#include "../db.test.h"
#include "sqlite/row.h"

#ifdef HAVE_LIBSQLITE3

using namespace bandit;

using namespace std;

using namespace arg3::db;

template <typename T>
shared_ptr<T> get_sqlite_row(size_t index)
{
    auto rs = current_session->query("select * from users");

    if (index > 0 && index >= rs.size()) {
        throw database_exception("not enough rows");
    }

    auto i = rs.begin();

    if (index > 0) {
        i += index;
    }

    if (i == rs.end()) {
        throw database_exception("no rows found");
    }

    return dynamic_pointer_cast<T>(i->impl());
}

go_bandit([]() {
    describe("a sqlite row", []() {

        auto sqlite_session = dynamic_pointer_cast<sqlite::session>(current_session->impl());

        before_each([]() {
            setup_current_session();

            user user1;

            user1.set("first_name", "Bryan");
            user1.set("last_name", "Jenkins");

            user1.save();

            user user2;

            user2.set("first_name", "Bob");
            user2.set("last_name", "Smith");

            user2.set("dval", 3.1456);

            user2.save();
        });

        after_each([]() { teardown_current_session(); });

        it("requires initializer parameters", [&sqlite_session]() {
            AssertThrows(database_exception, sqlite::row(nullptr, nullptr));

            AssertThrows(database_exception, sqlite::row(sqlite_session, nullptr));
        });

        it("is movable", [&sqlite_session]() {

            auto row = get_sqlite_row<sqlite::row>(0);

            Assert::That(row != nullptr, IsTrue());

            sqlite::row other(std::move(*row));

            Assert::That(other.is_valid(), IsTrue());

            row = get_sqlite_row<sqlite::row>(0);

            other = std::move(*row);

            Assert::That(other.is_valid(), IsTrue());
        });

        it("can handle invalid column selection", [&sqlite_session]() {

            auto row = get_sqlite_row<sqlite::row>(0);

            Assert::That(row != nullptr, IsTrue());

            AssertThrows(no_such_column_exception, row->column(1234));

            AssertThrows(no_such_column_exception, row->column(""));

            AssertThrows(no_such_column_exception, row->column("abracadabra"));

            AssertThrows(no_such_column_exception, row->column_name(1234));
        });
    });
});
#endif

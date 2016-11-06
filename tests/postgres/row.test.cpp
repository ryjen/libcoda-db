
#include <bandit/bandit.h>
#include "../db.test.h"
#include "postgres/row.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

shared_ptr<postgres::row> get_postgres_row(size_t index)
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
        throw database_exception("no row found");
    }

    return dynamic_pointer_cast<postgres::row>(i->impl());
}

go_bandit([]() {
    describe("a postgres row", []() {

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

        it("requires initializer parameters", []() {
            AssertThrows(database_exception, postgres::row(nullptr, nullptr, 0));

            AssertThrows(database_exception, postgres::row(dynamic_pointer_cast<postgres::session>(current_session->impl()), nullptr, 0));
        });

        it("is movable", []() {
            auto row = get_postgres_row(0);

            postgres::row other(std::move(*row));

            Assert::That(other.is_valid(), IsTrue());

            row = get_postgres_row(0);

            other = std::move(*row);

            Assert::That(other.is_valid(), IsTrue());
        });

        it("can handle invalid column selection", []() {
            auto row = get_postgres_row(0);

            AssertThrows(no_such_column_exception, row->column(1234));

            AssertThrows(no_such_column_exception, row->column(""));

            AssertThrows(no_such_column_exception, row->column_name(1234));
        });
    });
});

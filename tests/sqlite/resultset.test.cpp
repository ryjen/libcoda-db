
#include <bandit/bandit.h>
#include "../db.test.h"
#include "sqlite/resultset.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

shared_ptr<resultset_impl> get_sqlite_resultset()
{
    auto rs = test::current_session->query("select * from users");

    return rs.impl();
}

shared_ptr<resultset_impl> get_sqlite_cached_resultset()
{
    select_query query(test::current_session, {}, "users");

    auto rs = query.execute();

    return rs.impl();
}


template <typename T>
void test_move_resultset(const std::function<shared_ptr<resultset_impl>()> &funk)
{
    auto f1 = funk();

    auto f2 = funk();

    T c2(std::move(*dynamic_pointer_cast<T>(f1)));

    Assert::That(c2.is_valid(), IsTrue());

    c2 = std::move(*dynamic_pointer_cast<T>(f2));

    Assert::That(c2.is_valid(), IsTrue());
}

template <typename T>
void test_resultset_row(const std::function<shared_ptr<resultset_impl>()> &funk)
{
    auto r = funk();

    Assert::That(r->next(), IsTrue());

    Assert::That(r->current_row().is_valid(), IsTrue());

    Assert::That(r->current_row().size() > 0, IsTrue());
}

SPEC_BEGIN(sqlite_resultset)
{
    describe("sqlite resultset", []() {
        before_each([]() {
            test::setup_current_session();

            test::user user1;

            user1.set_id(1);
            user1.set("first_name", "Bryan");
            user1.set("last_name", "Jenkins");

            user1.save();

            test::user user2;

            user2.set_id(3);

            user2.set("first_name", "Bob");
            user2.set("last_name", "Smith");

            user2.set("dval", 3.1456);

            user2.save();
        });

        after_each([]() { test::teardown_current_session(); });

        auto sqlite_session = dynamic_pointer_cast<sqlite::session>(test::current_session->impl());

        it("is movable", [&sqlite_session]() { test_move_resultset<sqlite::resultset>(get_sqlite_resultset); });

        it("can get a row", [&sqlite_session]() { test_resultset_row<sqlite::resultset>(get_sqlite_resultset); });

        it("can handle a bad query", []() {
            AssertThat(test::current_session->execute("select * from asdfasdfasdf"), Equals(false));

            select_query query(test::current_session, {}, "asdfasdfasdf");

            AssertThrows(database_exception, query.execute());
        });
    });
}
SPEC_END;

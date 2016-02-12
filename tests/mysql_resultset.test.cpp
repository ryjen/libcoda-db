#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#undef VERSION

#if defined(HAVE_LIBMYSQLCLIENT) && defined(TEST_MYSQL)

#include <bandit/bandit.h>
#include "db.test.h"
#include "mysql/resultset.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

shared_ptr<resultset_impl> get_resultset()
{
    auto rs = mysql_testdb.execute("select * from users");

    return rs.impl();
}

shared_ptr<resultset_impl> get_stmt_resultset()
{
    select_query query(&mysql_testdb, "users");

    auto rs = query.execute();

    return rs.impl();
}


template <typename T>
void test_move_resultset(std::function<shared_ptr<resultset_impl>()> funk)
{
    auto f1 = funk();

    auto f2 = funk();

    auto f1Value = f1->size();

    auto f2Value = f2->size();

    T c2(std::move(*static_pointer_cast<T>(f1)));

    Assert::That(c2.is_valid(), IsTrue());

    Assert::That(c2.size() == f1Value, IsTrue());

    c2 = std::move(*static_pointer_cast<T>(f2));

    Assert::That(c2.is_valid(), IsTrue());

    Assert::That(c2.size() == f2Value, IsTrue());
}

template <typename T>
void test_resultset_row(std::function<shared_ptr<resultset_impl>()> funk)
{
    auto r = funk();

    Assert::That(r->next(), IsTrue());

    Assert::That(r->current_row().is_valid(), IsTrue());

    Assert::That(r->current_row().size() > 0, IsTrue());
}

go_bandit([]() {

    describe("mysql row", []() {
        before_each([]() {
            mysql_testdb.setup();

            user user1(&mysql_testdb);

            user1.set_id(1);
            user1.set("first_name", "Bryan");
            user1.set("last_name", "Jenkins");

            user1.save();

            user user2(&mysql_testdb);

            user2.set_id(3);

            user2.set("first_name", "Bob");
            user2.set("last_name", "Smith");

            user2.set("dval", 3.1456);

            user2.save();
        });

        after_each([]() { mysql_testdb.teardown(); });

        describe("is movable", []() {


            it("as statement results", []() { test_move_resultset<mysql::stmt_resultset>(get_stmt_resultset); });

            it("as results", []() { test_move_resultset<mysql::resultset>(get_resultset); });


        });

        describe("can get a row", []() {

            it("as statement results", []() {
                test_resultset_row<mysql::stmt_resultset>(get_stmt_resultset);

            });

            it("as results", []() { test_resultset_row<mysql::resultset>(get_resultset); });

        });

        it("can handle a bad query", []() {
            AssertThrows(database_exception, mysql_testdb.execute("select * from asdfasdfasdf"));

            select_query query(&mysql_testdb, "asdfasdfasdf");

            AssertThrows(database_exception, query.execute());
        });
    });

});

#endif

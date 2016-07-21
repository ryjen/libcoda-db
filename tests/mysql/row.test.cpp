#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#undef VERSION

#ifdef HAVE_LIBMYSQLCLIENT

#include <bandit/bandit.h>
#include "../db.test.h"
#include "mysql/row.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

shared_ptr<row_impl> get_mysql_results_row(size_t index)
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
        throw database_exception("No row found");
    }

    return i->impl();
}

shared_ptr<row_impl> get_mysql_stmt_row(size_t index)
{
    select_query query(current_session, {}, "users");

    auto rs = query.execute();

    if (index > 0 && index >= rs.size()) {
        throw database_exception("not enough rows");
    }

    auto i = rs.begin();

    if (index > 0) {
        i += index;
    }

    if (i == rs.end()) {
        throw database_exception("No row found");
    }

    return i->impl();
}

template <typename T>
void test_move_row(std::function<shared_ptr<row_impl>(size_t)> funk)
{
    auto f1 = funk(0);

    auto f2 = funk(1);

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
void test_row_column(std::function<shared_ptr<row_impl>(size_t)> funk)
{
    auto r = funk(0);

    Assert::That(r->is_valid(), IsTrue());

    Assert::That(r->column(0).is_valid(), IsTrue());

    Assert::That(r->column(1).to_value(), Equals("Bryan"));

    AssertThrows(database_exception, r->column(1234));

    AssertThrows(database_exception, r->column("absdfas"));
}

go_bandit([]() {

    describe("mysql row", []() {
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

        describe("is movable", []() {

            it("as statement results", []() { test_move_row<mysql::stmt_row>(get_mysql_stmt_row); });

            it("as results", []() { test_move_row<mysql::row>(get_mysql_results_row); });

        });

        describe("can get a column name", []() {

            it("as statement results", []() {
                select_query query(current_session, {}, "users");
                auto rs = query.execute();
                auto c = rs.begin();

                Assert::That(c->column_name(0), Equals("id"));
                AssertThrows(database_exception, c->column_name(1234123));
            });

            it("as results", []() {
                auto c = get_mysql_results_row(0);

                Assert::That(c->column_name(0), Equals("id"));

                AssertThrows(database_exception, c->column_name(1234123));
            });

        });

        describe("can get a column", []() {

            it("as statement results", []() { test_row_column<mysql::stmt_row>(get_mysql_stmt_row); });

            it("as results", []() { test_row_column<mysql::row>(get_mysql_results_row); });

        });
    });

});

#endif

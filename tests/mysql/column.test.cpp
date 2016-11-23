
#include <bandit/bandit.h>
#include "../db.test.h"
#include "mysql/column.h"

using namespace bandit;

using namespace std;

using namespace rj::db;

shared_ptr<column_impl> get_results_column(size_t index, size_t iterations)
{
    auto rs = test::current_session->query("select * from users");

    if (iterations > 0 && iterations >= rs.size()) {
        throw database_exception("not enough rows");
    }

    auto i = rs.begin();

    if (iterations > 0) {
        i += iterations;
    }

    if (index >= i->size()) throw database_exception("invalid column index");

    auto c = i->column(index);

    return c.impl();
}

shared_ptr<column_impl> get_stmt_column(size_t index, size_t iterations)
{
    select_query query(test::current_session, {}, "users");

    auto rs = query.execute();

    if (iterations > 0 && iterations >= rs.size()) {
        throw database_exception("not enough rows");
    }

    auto i = rs.begin();

    if (iterations > 0) {
        i += iterations;
    }

    if (index >= i->size()) throw database_exception("invalid column index");

    auto c = i->column(index);

    return c.impl();
}

template <typename T>
void test_move_column(std::function<shared_ptr<column_impl>(size_t, size_t)> funk)
{
    auto f1 = funk(0, 0);

    auto f2 = funk(1, 0);

    auto f1Value = f1->to_value();

    auto f2Value = f2->to_value();

    T c2(std::move(*static_pointer_cast<T>(f1)));

    Assert::That(c2.is_valid(), IsTrue());

    Assert::That(c2.to_value() == f1Value, IsTrue());

    c2 = std::move(*static_pointer_cast<T>(f2));

    Assert::That(c2.is_valid(), IsTrue());

    Assert::That(c2.to_value() == f2Value, IsTrue());
}

SPEC_BEGIN(mysql_column)
{
    describe("mysql column", []() {

        before_each([]() {

            test::setup_current_session();

            test::user user1;

            user1.set_id(1);
            user1.set("first_name", "Bryan");
            user1.set("last_name", "true");

            unsigned char* ptr = (unsigned char*)calloc(4, sizeof(int));

            sql_blob data(ptr, ptr + (4 * sizeof(int)));

            user1.set("data", data);

            user1.set("dval", 3.1456);

            user1.save();

            free(ptr);

            test::user user2;

            user2.set_id(3);

            user2.set("first_name", "Bob");
            user2.set("last_name", "false");

            user2.set("data", data);

            user2.set("dval", 3.1456);

            user2.save();
        });

        after_each([]() { test::teardown_current_session(); });

        describe("is movable", []() {

            it("as statement results", []() { test_move_column<mysql::stmt_column>(get_stmt_column); });

            it("as results", []() { test_move_column<mysql::column>(get_results_column); });

        });

        describe("has a name", []() {

            it("as statement results", []() {
                auto c = get_stmt_column(0, 0);

                Assert::That(c->name(), Equals("id"));
            });

            it("as results", []() {
                auto c = get_results_column(0, 0);

                Assert::That(c->name(), Equals("id"));
            });

        });

        describe("can return a blob", []() {

            it("as statement results", []() {
                auto c = get_stmt_column(4, 0);

                Assert::That(static_pointer_cast<mysql::stmt_column>(c)->sql_type(), Equals(MYSQL_TYPE_BLOB));

                sql_blob b = c->to_value();

                Assert::That(b.size(), Equals(4 * sizeof(int)));
            });

            it("as results", []() {
                auto c = get_results_column(4, 0);

                Assert::That(static_pointer_cast<mysql::column>(c)->sql_type(), Equals(MYSQL_TYPE_BLOB));

                sql_blob b = c->to_value();

                Assert::That(b.size(), Equals(4 * sizeof(int)));
            });

        });

        describe("can return a double", []() {

            it("as statement results", []() {
                auto c = get_stmt_column(3, 0);

                Assert::That(static_pointer_cast<mysql::stmt_column>(c)->sql_type(), Equals(MYSQL_TYPE_DOUBLE));
                Assert::That(c->to_value(), Equals(3.1456));

                c = get_stmt_column(1, 0);

                AssertThrows(value_conversion_error, c->to_value().as<double>());
            });

            it("as results", []() {
                auto c = get_results_column(3, 0);

                Assert::That(static_pointer_cast<mysql::column>(c)->sql_type(), Equals(MYSQL_TYPE_DOUBLE));
                Assert::That(c->to_value(), Equals(3.1456));

                c = get_results_column(1, 0);

                AssertThrows(value_conversion_error, c->to_value().as<double>());
            });

        });

        describe("can return an int", []() {

            it("as statement results", []() {
                auto c = get_stmt_column(3, 0);

                Assert::That(c->to_value(), Equals(3));

                c = get_stmt_column(1, 0);

                AssertThrows(value_conversion_error, c->to_value().as<int>());
            });

            it("as results", []() {
                auto c = get_results_column(3, 0);

                Assert::That(c->to_value(), Equals(3));

                c = get_results_column(1, 0);

                AssertThrows(value_conversion_error, c->to_value().as<int>());
            });

        });

        describe("can return an long long int", []() {

            it("as statement results", []() {
                auto c = get_stmt_column(3, 0);

                Assert::That(c->to_value(), Equals(3));

                c = get_stmt_column(1, 0);

                AssertThrows(value_conversion_error, c->to_value().as<long long>());
            });

            it("as results", []() {
                auto c = get_results_column(3, 0);

                Assert::That(c->to_value(), Equals(3));

                c = get_results_column(1, 0);

                AssertThrows(value_conversion_error, c->to_value().as<long long>());
            });

        });

        describe("can return a bool", []() {

            it("as statement results", []() {
                auto c = get_stmt_column(2, 0);

                Assert::That(c->to_value(), IsTrue());

                c = get_stmt_column(1, 0);

                Assert::That(c->to_value(), IsFalse());

                c = get_stmt_column(2, 1);

                Assert::That(c->to_value(), IsFalse());
            });

            it("as results", []() {
                auto c = get_results_column(2, 0);

                Assert::That(c->to_value(), IsTrue());

                c = get_results_column(1, 0);

                Assert::That(c->to_value(), IsFalse());

                c = get_results_column(2, 1);

                Assert::That(c->to_value(), IsFalse());
            });

        });
    });
}
SPEC_END;

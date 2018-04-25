#include <string>

#include <bandit/bandit.h>
#include "db.test.h"
#include "record.h"
#include "sqlite/column.h"

using namespace bandit;

using namespace std;

using namespace coda::db;

using namespace snowhouse;

column get_user_column(const string &name)
{
    select_query q(test::current_session);

    q.from("users");

    auto rs = q.execute();

    if (!rs.is_valid()) {
        throw database_exception("no rows in test");
    }

    auto row = rs.begin();

    if (row == rs.end() || !row->is_valid()) {
        throw database_exception("no rows in test");
    }

    return row->column(name);
}

specification(columns, []() {
    describe("column", []() {
        before_each([]() { test::setup_current_session(); });

        after_each([]() { test::teardown_current_session(); });

        before_each([]() {
            test::user u;
            u.set("first_name", "Bob");
            u.set("last_name", "Jenkins");
            u.set("dval", 123.321);

            int *value = new int;

            *value = 4;

            sql_blob data(value, sizeof(int));

            delete value;

            u.set("data", data);

            u.set("tval", sql_time());

            u.save();
        });

        it("is copyable", []() {
            auto col = get_user_column("first_name");

            column other(col);

            AssertThat(other.is_valid(), IsTrue());

            AssertThat(other.value(), Equals(col.value()));
        });

        it("is movable", []() {
            auto col = get_user_column("first_name");

            auto val = col.value();

            column &&other(std::move(col));

            AssertThat(other.is_valid(), IsTrue());

            AssertThat(other.value(), Equals(val));

            column &&last = get_user_column("last_name");

            last = std::move(other);

            AssertThat(other.is_valid(), IsFalse());
            AssertThat(last.is_valid(), IsTrue());
            AssertThat(last.value(), Equals(val));
        });

        it("can be a blob", []() {
            auto col = get_user_column("data");

            AssertThat(col.value().is<sql_blob>(), IsTrue());

            auto blob = col.value().as<sql_blob>();

            AssertThat(blob.size(), Equals(sizeof(int)));

            int* p = static_cast<int*>(blob.get());

            AssertThat(*p, Equals(4));
        });

        it("can be a time", []() {
            auto col = get_user_column("tval");

            AssertThat(col.value().as<sql_time>().value() > 0, IsTrue());
        });

        it("can be a double", []() {
            auto col = get_user_column("dval");

            AssertThat(col.value().as<double>(), Equals(123.321));

            double val = col;

            AssertThat(val, Equals(123.321));
        });

        it("can be a float", []() {
            auto col = get_user_column("dval");

            AssertThat(col.value().as<float>(), Equals(123.321f));

            float val = col;

            AssertThat(val, Equals(123.321f));
        });

        it("can be an int64", []() {
            auto col = get_user_column("id");

            AssertThat(col.value().as<long long>() > 0, IsTrue());

            long long val = col;

            AssertThat(val > 0, IsTrue());
        });

        it("can be an unsigned int", []() {
            auto col = get_user_column("id");

            AssertThat(col.value().as<unsigned int>() > 0, IsTrue());

            unsigned val = col;

            AssertThat(val > 0, IsTrue());

            unsigned long long val2 = col;

            AssertThat(val2 > 0, IsTrue());
        });

        it("can be a string", []() {
            auto col = get_user_column("first_name");

            AssertThat(col.value(), Equals("Bob"));

            std::string val = col;

            AssertThat(val, Equals("Bob"));
        });
    });
});

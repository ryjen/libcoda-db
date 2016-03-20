#include <bandit/bandit.h>
#include "record.h"
#include "db.test.h"
#include "sqlite/column.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;


column get_user_column(const string &name)
{
    select_query q(current_session);

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

go_bandit([]() {
    describe("column", []() {
        before_each([]() { setup_current_session(); });

        after_each([]() { teardown_current_session(); });

        before_each([]() {
            user u;
            u.set("first_name", "Bob");
            u.set("last_name", "Jenkins");
            u.set("dval", 123.321);

            int *data = (int *)malloc(sizeof(int));

            memset(data, 0, sizeof(int));

            *data = 4;

            u.set("data", sql_blob(data, sizeof(int)));

            u.set("tval", sql_time());

            u.save();

            free(data);
        });

        it("is copyable", []() {
            auto col = get_user_column("first_name");

            column other(col);

            AssertThat(other.is_valid(), IsTrue());

            AssertThat(other.to_value(), Equals(col.to_value()));
        });

        it("is movable", []() {
            auto col = get_user_column("first_name");

            auto val = col.to_value();

            column &&other(std::move(col));

            AssertThat(other.is_valid(), IsTrue());

            AssertThat(other.to_value(), Equals(val));

            column &&last = get_user_column("last_name");

            last = std::move(other);

            AssertThat(other.is_valid(), IsFalse());
            AssertThat(last.is_valid(), IsTrue());
            AssertThat(last.to_value(), Equals(val));
        });

        it("can be a blob", []() {
            auto col = get_user_column("data");

            AssertThat(col.to_value().is_binary(), IsTrue());

            AssertThat(col.to_blob().size(), Equals(sizeof(int)));
        });

        it("can be a time", []() {
            auto col = get_user_column("tval");

            AssertThat(col.to_time().to_uint() > 0, IsTrue());
        });

        it("can be a double", []() {
            auto col = get_user_column("dval");

            AssertThat(col.to_value(), Equals(123.321));

            double val = col;

            AssertThat(val, Equals(123.321));
        });

        it("can be a float", []() {
            auto col = get_user_column("dval");

            AssertThat(col.to_value(), Equals(123.321f));

            float val = col;

            AssertThat(val, Equals(123.321f));
        });

        it("can be an int64", []() {
            auto col = get_user_column("id");

            AssertThat(col.to_value().to_llong() > 0, IsTrue());

            long long val = col;

            AssertThat(val > 0, IsTrue());
        });

        it("can be an unsigned int", []() {
            auto col = get_user_column("id");

            AssertThat(col.to_value().to_uint() > 0, IsTrue());

            unsigned val = col;

            AssertThat(val > 0, IsTrue());

            unsigned long long val2 = col;

            AssertThat(val > 0, IsTrue());
        });

        it("can be a string", []() {
            auto col = get_user_column("first_name");

            AssertThat(col.to_value(), Equals("Bob"));

            std::string val = col;

            AssertThat(val, Equals("Bob"));
        });
    });
});

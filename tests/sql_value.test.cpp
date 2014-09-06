#include <bandit/bandit.h>
#include "sql_value.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

go_bandit([]()
{
    describe("sql value", []()
    {
        it("has equality", []()
        {
            sql_value v = "1234";

            Assert::That(v, Equals(1234));

            Assert::That(v, Equals("1234"));

            sql_value a = "1234";

            sql_value b = 1234;

            Assert::That(a, Equals(b));

            sql_value c = "asdfasdf";

            AssertThat(a, !Equals(c));
        });

        it("can be moved", []()
        {

            sql_value v = 1234;

            sql_value other(std::move(v));

            AssertThat(other != v, IsTrue());

            AssertThat(other == 1234, IsTrue());
        });

        it("can be converted", []()
        {
            sql_value v = 1234;

            double d = v;

            AssertThat(d, Equals(1234.0));

            long long i64 = v;

            AssertThat(i64, Equals(1234));

            string str = v;

            AssertThat(str, Equals("1234"));

            bool b = v;

            AssertThat(b, IsTrue());

            sql_blob blob = v;

            AssertThat(blob.ptr() == NULL, IsTrue());
        });

        it("can be converted without error", []()
        {
            sql_value v = "asdfb";

            double d = v;

            AssertThat(d, Equals(0.0));

            long long i64 = v;

            AssertThat(i64, Equals(0LL));

            bool b = v;

            AssertThat(b, IsFalse());

            sql_value boolTest = "true";

            b = boolTest;

            AssertThat(b, IsTrue());

            boolTest = "YES";

            b = boolTest;

            AssertThat(b, IsTrue());

            int i = v;

            AssertThat(i, Equals(0));

        });

    });

    describe("sql null", []()
    {
        it("can be a string", []()
        {
            AssertThat(to_string(sql_null), Equals("NULL"));
        });

        it("can be a sql value", []()
        {
            sql_value value = sql_null;

            AssertThat(value, Equals(sql_null));
        });
    });

    describe("sql blob", []()
    {
        it("can be a string", []()
        {
            sql_blob blob(NULL, 0);

            AssertThat(to_string(blob), Equals("0x0"));

            sql_blob other(&blob, sizeof(sql_blob));

            char buf[100];

            sprintf(buf, "%p", &blob);

            AssertThat(to_string(other), Equals(string(buf)));
        });

        it("can be moved", []()
        {
            int a = 1234;

            sql_blob b(&a, sizeof(int));

            sql_blob other(std::move(b));

            AssertThat(b.ptr() == &a, IsFalse());

            AssertThat(other.ptr() == &a, IsTrue());

            sql_blob moved(NULL, 0);

            moved = std::move(other);

            AssertThat(other.ptr() == &a, IsFalse());

            AssertThat(moved.ptr() == &a, IsTrue());
        });

    });

});


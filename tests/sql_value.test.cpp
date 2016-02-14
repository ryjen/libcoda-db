#include <bandit/bandit.h>
#include "sql_value.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

go_bandit([]() {

    describe("sql time value", []() {

        struct tm tm;
        tm.tm_year = 2016 - 1900;
        tm.tm_mon = 2;
        tm.tm_mday = 11;
        tm.tm_hour = 8;
        tm.tm_min = 15;
        tm.tm_sec = 30;

        it("has equality", []() {
            time_t curr = time(0);

            sql_time value(curr);

            sql_time other(curr);

            Assert::That(value, Equals(other));

            other = 12341234U;

            Assert::That(value, !Equals(other));

        });

        it("can be converted", [&tm]() {

            time_t curr = timegm(&tm);

            sql_time value(curr);

            Assert::That(value.to_ulong(), Equals(curr));

            Assert::That(value.to_llong(), Equals(curr));

            Assert::That(value.to_ullong(), Equals(curr));

            Assert::That(value.to_bool(), IsTrue());

            auto lt = value.to_localtime();

            Assert::That(lt->tm_mon == 2, IsTrue());
        });

        it("has different formats", [&tm]() {


            time_t current = timegm(&tm);

            sql_time date(current, sql_time::DATE);

            Assert::That(date.to_string(), Equals("2016-03-11"));

            sql_time tim(current, sql_time::TIME);

            Assert::That(tim.to_string(), Equals("08:15:30"));

            sql_time timestamp(current, sql_time::TIMESTAMP);

            Assert::That(timestamp.to_string(), Equals("2016-03-11 08:15:30"));
        });

        it("can be outputed to a stream", [&tm]() {
            sql_time value(timegm(&tm));

            ostringstream out;

            out << value;

            Assert::That(out.str(), Equals("2016-03-11 08:15:30"));

        });
    });
    describe("sql value", []() {
        it("has equality", []() {
            sql_value v = "1234";

            Assert::That(v, Equals(1234));

            Assert::That(v, Equals("1234"));

            sql_value a = "12345";

            sql_value b = 12345;

            Assert::That(a, Equals(b));

            sql_value c = "asdfasdf";

            AssertThat(a, !Equals(c));
        });

        it("can be moved", []() {

            sql_value v = 1234;

            sql_value other(std::move(v));

            AssertThat(other != v, IsTrue());

            AssertThat(other == 1234, IsTrue());
        });

        it("can be converted", []() {
            sql_value v = 1234;

            double d = v;

            AssertThat(d, Equals(1234.0));

            long long i64 = v;

            AssertThat(i64, Equals(1234));

            string str = v;

            AssertThat(str, Equals("1234"));

            bool b = v;

            AssertThat(b, IsTrue());

        });

        /*
         * Not sure I want this test case anymore... conversion errors should be raised
         *
        it("can be converted without error", []() {
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

        });*/

        it("throws errors on conversion", []() {
            sql_value v = "asdfcv";

            AssertThrows(arg3::illegal_conversion, v.to_double());
        });


        it("can be outputed to a stream", []() {
            sql_value value(12341234);

            ostringstream buf;

            buf << value;

            Assert::That(buf.str(), Equals("12341234"));
        });


    });

    describe("sql null", []() {
        it("can be a string", []() { AssertThat(to_string(sql_null), Equals("NULL")); });

        it("can be a sql value", []() {
            sql_value value;

            AssertThat(value == sql_null, IsTrue());
        });

        it("can be outputed to a stream", []() {
            ostringstream buf;

            buf << sql_null;

            Assert::That(buf.str(), Equals("NULL"));
        });
    });

    describe("sql blob", []() {
        it("can be a string", []() {
            sql_blob blob;

            AssertThat(to_string(blob), Equals("0x0"));

            sql_blob other(&blob, sizeof(sql_blob));

            char buf[100];

            sprintf(buf, "%p", other.value());

            AssertThat(to_string(other), Equals(string(buf)));
        });

        it("can be moved", []() {
            int a = 1234;

            sql_blob b(&a, sizeof(int));

            sql_blob other(std::move(b));

            AssertThat(b.is_null(), IsTrue());

            int* x = static_cast<int*>(other.value());

            AssertThat(*x == a, IsTrue());

            sql_blob moved;

            moved = std::move(other);

            AssertThat(other.is_null(), IsTrue());

            x = static_cast<int*>(moved.value());

            AssertThat(*x == a, IsTrue());
        });

    });

});

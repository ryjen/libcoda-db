#include <string>

#include <bandit/bandit.h>
#include "exception.h"
#include "sql_value.h"

using namespace bandit;

using namespace std;

using namespace coda::db;

using namespace snowhouse;

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

            other = sql_time(12341234U);

            Assert::That(value, !Equals(other));

        });

        it("can be converted", [&tm]() {

            time_t curr = timegm(&tm);

            sql_time value(curr);

            Assert::That(value.value(), Equals(curr));

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

            AssertThat(other == 1234, IsTrue());
        });

        describe("conversion", []() {

            describe("an integer", []() {
                sql_value v = 1234;

                it("as a double", [&v]() {
                    double d = v.as<double>();

                    AssertThat(d, Equals(1234.0));
                });

                it("as a long long", [&v]() {
                    long long i64 = v.as<long long>();

                    AssertThat(i64, Equals(1234));
                });

                it("as a string", [&v]() {
                    sql_string str = v.as<sql_string>();

                    AssertThat(str, Equals("1234"));
                });

                it("as a bool", [&v]() {
                    bool b = v.as<bool>();

                    AssertThat(b, IsTrue());
                });
            });

            describe("a string", []() {

                it("as a conversion error", []() {
                    sql_value v = "asdfcv";

                    AssertThrows(coda::db::value_conversion_error, v.as<int>());
                });

                it("as a sql_time", []() {
                    sql_value v = "2016-06-12 10:32:23";

                    sql_time t = v.as<sql_time>();

                    AssertThat(t.value(), Equals(1465727543));

                    AssertThat(t.format(), Equals(sql_time::DATETIME));

                    v = "2016-06-12";

                    t = v.as<sql_time>();

                    AssertThat(t.value(), Equals(1465689600));

                    AssertThat(t.format(), Equals(sql_time::DATE));

                    v = "10:32:23";

                    t = v.as<sql_time>();

                    AssertThat(t.value(), Equals(37943));

                    AssertThat(t.format(), Equals(sql_time::TIME));
                });
            });

            // describe("a blob", []() {
            //     it("is not a sql time", []() {
            //         std::string abc = "abc123";
            //         sql_value v(sql_blob(abc.begin(), abc.end()));
            //         AssertThrows(coda::db::value_conversion_error, v.as<sql_blob>());
            //     });
            // });
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
        it("can be a wide string", []() { AssertThat(to_wstring(sql_null), Equals(std::wstring(L"NULL"))); });

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

            AssertThat(to_string(blob), Equals("NULL"));

            AssertThat(to_wstring(blob), Equals(std::wstring(L"NULL")));

            std::string data("123456");

            sql_blob other(data.data(), data.size());

            std::ostringstream buf;

            buf << std::hex << (intptr_t) other.get();

            AssertThat(to_string(other), Equals(buf.str()));

            std::wostringstream wbuf;

            wbuf << std::hex << (intptr_t) other.get();

            AssertThat(to_wstring(other), Equals(wbuf.str()));
        });

    });
});

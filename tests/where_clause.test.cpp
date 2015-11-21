#include <bandit/bandit.h>
#include "where_clause.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

go_bandit([]() {

    describe("where clause", []() {

        it("can and and or", []() {
            where_clause w("this");

            w && "that";

            w || "blah";

            AssertThat(w.to_string(), Equals("this AND that OR blah"));
        });

        it("can operate with other clauses", []() {
            where_clause w1("this");

            where_clause w2("that");

            where_clause w3("blah");

            where_clause w4("bleh");

            w1 || w2;

            string value = (string)w1;

            AssertThat(value, Equals("this OR that"));

            w3&& w4;

            AssertThat(w3.to_string(), Equals("blah AND bleh"));
        });

        it("can operate on empty", []() {
            where_clause w1;

            where_clause w2;

            w1 && "blah";

            string value = (string)w1;

            AssertThat(value, Equals("blah"));

            w2 || "bleh";

            AssertThat(w2.to_string(), Equals("bleh"));

            where_clause w3;

            w3&& w1;

            AssertThat(w3.to_string(), Equals(w1.to_string()));

            where_clause w4;

            w4 || w1;

            AssertThat(w4.to_string(), Equals(w1.to_string()));
        });

        it("can reset", []() {
            where_clause w("this");

            w || "that";

            w.reset();

            AssertThat(w.to_string(), Equals(""));

        });

        it("can use literal", []() {
            auto w = ("this = ?"_w && "that = ?"_w);

            w || ("abc = def"_w && "xyz = tuv"_w);

            AssertThat(w.to_string(), Equals("this = ? AND that = ? OR abc = def AND xyz = tuv"));
        });

    });

});

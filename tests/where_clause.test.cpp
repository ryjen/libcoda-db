#include <string>

#include "where_clause.h"
#include <bandit/bandit.h>

using namespace bandit;

using namespace std;

using namespace coda::db;

using namespace snowhouse;

go_bandit([]() {
  describe("where clause", []() {
    it("can and and or", []() {
      auto w = (where("this") and "that") or (where("blah") and "bleh");

      AssertThat(w.to_sql(), Equals("(this AND that) OR (blah AND bleh)"));
    });

    it("can operate with other clauses", []() {
      where_clause w1("this");

      where_clause w2("that");

      where_clause w3("blah");

      where_clause w4("bleh");

      w1 or w2;

      string value = (string) w1;

      AssertThat(value, Equals("this OR that"));

      w3 && w4;

      AssertThat(w3.to_sql(), Equals("blah AND bleh"));
    });

    it("can operate on empty", []() {
      where_clause w1;

      where_clause w2;

      w1 and "blah";

      string value = (string) w1;

      AssertThat(value, Equals("blah"));

      w2 or "bleh";

      AssertThat(w2.to_sql(), Equals("bleh"));

      where_clause w3;

      w3 && w1;

      AssertThat(w3.to_sql(), Equals(w1.to_sql()));

      where_clause w4;

      w4 or w1;

      AssertThat(w4.to_sql(), Equals(w1.to_sql()));
    });

    it("can reset", []() {
      where_clause w("this");

      w or "that";

      w.reset();

      AssertThat(w.to_sql(), Equals(""));
    });

    it("can combine", []() {
      auto w = where("this = $1") and ("that = $2");

      w or (where("abc = def") and "xyz = tuv");

      AssertThat(
          w.to_sql(),
          Equals("(this = $1 AND that = $2) OR (abc = def AND xyz = tuv)"));
    });
  });

  describe("sql operator", []() {
    describe("builder", []() {
      struct visitor {
        void operator()(const sql_value &rvalue) const {
          AssertThat(rvalue, Equals(1234));
        }
        void operator()(const std::vector<sql_value> &rvalue) const {
          AssertThat(false, IsTrue());
        }
        void operator()(const std::pair<sql_value, sql_value> &rvalue) const {
          AssertThat(false, IsTrue());
        }
      };
      auto builder = ("test"_op = 1234);
      AssertThat(builder.lvalue(), Equals("test"));
      builder.rvalue(visitor());
      AssertThat(builder.type(), Equals(op::EQ));
    });
  });
});

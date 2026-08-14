#include <string>

#include <bandit/bandit.h>
#include "schema.h"
#include "session.h"
#include "where_clause.h"

using namespace bandit;
using namespace std;
using namespace coda::db;
using namespace snowhouse;

namespace {
  class test_bindable : public bindable {
   public:
    bindable &bind(size_t, const sql_value &) override {
      ++count_;
      return *this;
    }

    bindable &bind(const std::string &, const sql_value &) override {
      ++count_;
      return *this;
    }

    size_t num_of_bindings() const noexcept override {
      return count_;
    }

   private:
    size_t count_ = 0;
  };

  class test_session : public session_impl {
   public:
    test_session() : session_impl(uri()) {}

    bool is_open() const noexcept override {
      return false;
    }

    void open() override {}

    void close() override {}

    sql_id last_insert_id() const override {
      return 0;
    }

    sql_changes last_number_of_changes() const override {
      return 0;
    }

    std::string last_error() const override {
      return "";
    }

    std::shared_ptr<resultset_type> query(const std::string &) override {
      return nullptr;
    }

    bool execute(const std::string &) override {
      return false;
    }

    std::shared_ptr<statement_type> create_statement() override {
      return nullptr;
    }

    std::shared_ptr<transaction_type> create_transaction() const override {
      return nullptr;
    }

    std::vector<column_definition> get_columns_for_schema(const std::string &, const std::string &) override {
      return {};
    }

    std::string bind_param(size_t index) const override {
      return "$" + std::to_string(index);
    }
  };
}

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

      w3 &&w4;

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

      w3 &&w1;

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

      AssertThat(w.to_sql(), Equals("(this = $1 AND that = $2) OR (abc = def AND xyz = tuv)"));
    });

    it("can combine builder operators with or", []() {
      test_bindable binder;
      auto session = std::make_shared<test_session>();
      where_builder w(session, &binder);

      w.reset("first"_op = 1);
      w || ("second"_op = 2);

      AssertThat(w.to_sql(), Equals("first = $1 OR second = $2"));
      AssertThat(binder.num_of_bindings(), Equals(2u));
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

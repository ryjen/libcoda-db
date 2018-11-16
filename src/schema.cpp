
#include "schema.h"
#include "exception.h"
#include "resultset.h"
#include "session.h"
#include "sql_value.h"
#include "sqldb.h"

using namespace std;

namespace coda::db {
    ostream &operator<<(ostream &os, const column_definition &def) {
      os << def.name;
      return os;
    }

    schema::schema(const std::shared_ptr<coda::db::session> &session, const string &tableName)
        : session_(session), tableName_(tableName) {
      if (session_ == nullptr) {
        throw database_exception("no database provided for schema");
      }

      if (tableName_.empty()) {
        throw database_exception("no table name provided for schema");
      }
    }

    bool schema::is_valid() const noexcept { return !columns_.empty(); }

    void schema::init() {
      if (!session_->is_open()) {
        throw database_exception("database is not open");
      }

      columns_ = session_->get_columns_for_schema(tableName_);
    }

    vector<column_definition> schema::columns() const noexcept { return columns_; }

    vector<string> schema::column_names() const {
      vector<string> names;

      for (auto &c : columns_) {
        names.push_back(c.name);
      }
      return names;
    }

    vector<string> schema::primary_keys() const {
      vector<string> names;

      for (auto &c : columns_) {
        if (c.pk) {
          names.push_back(c.name);
        }
      }

      return names;
    }

    std::string schema::primary_key() const {
      for (auto &c : columns_) {
        if (c.pk && c.autoincrement) {
          return c.name;
        }
      }

      throw no_primary_key_exception("no primary key found for schema");
    }

    sql_value schema::default_value(const std::string &name) const {
      for (auto &c : columns_) {
        if (c.name == name) {
          return c.default_value;
        }
      }

      return sql_value();
    }

    string schema::table_name() const { return tableName_; }

    std::shared_ptr<session> schema::get_session() const { return session_; }

    column_definition schema::operator[](size_t index) const { return columns_[index]; }

    size_t schema::size() const noexcept { return columns_.size(); }
}  // namespace coda::db

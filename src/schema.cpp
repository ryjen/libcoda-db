
#include "schema.h"
#include "exception.h"
#include "resultset.h"
#include "session.h"
#include "sql_value.h"
#include "sqldb.h"

using namespace std;

namespace coda {
  namespace db {
    ostream &operator<<(ostream &os, const column_definition &def) {
      os << def.name;
      return os;
    }

    schema::schema(const std::shared_ptr<coda::db::session> &session,
                   const string &tablename)
        : session_(session), tableName_(tablename) {
      if (session_ == nullptr) {
        throw database_exception("no database provided for schema");
      }

      if (tableName_.empty()) {
        throw database_exception("no table name provided for schema");
      }
    }

    schema::~schema() {}

    schema::schema(const schema &other)
        : session_(other.session_), tableName_(other.tableName_),
          columns_(other.columns_) {}

    schema::schema(schema &&other)
        : session_(std::move(other.session_)),
          tableName_(std::move(other.tableName_)),
          columns_(std::move(other.columns_)) {
      other.session_ = nullptr;
      other.columns_.clear();
    }

    schema &schema::operator=(const schema &other) {
      columns_ = other.columns_;
      session_ = other.session_;
      tableName_ = other.tableName_;

      return *this;
    }

    schema &schema::operator=(schema &&other) {
      columns_ = std::move(other.columns_);
      session_ = std::move(other.session_);
      tableName_ = std::move(other.tableName_);

      other.columns_.clear();
      other.session_ = nullptr;

      return *this;
    }

    bool schema::is_valid() const noexcept { return columns_.size() > 0; }

    void schema::init() {
      if (!session_->is_open()) {
        throw database_exception("database is not open");
      }

      columns_ = session_->get_columns_for_schema(tableName_);
    }

    vector<column_definition> schema::columns() const noexcept {
      return columns_;
    }

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

    column_definition schema::operator[](size_t index) const {
      return columns_[index];
    }

    size_t schema::size() const noexcept { return columns_.size(); }
  }; // namespace db
} // namespace coda

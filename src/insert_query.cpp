#include "insert_query.h"
#include "schema.h"
#include "session.h"
#include "statement.h"

using namespace std;

namespace coda {
  namespace db {
    /*!
     * @param db the database to modify
     * @param tableName the table to modify
     * @param columns the columns to modify
     */
    insert_query::insert_query(
        const std::shared_ptr<coda::db::session> &session,
        const std::string &tableName)
        : modify_query(session) {
      tableName_ = tableName;
    }

    /*!
     * @param db the database to modify
     * @param columns the columns to modify
     */
    insert_query::insert_query(
        const std::shared_ptr<coda::db::session> &session,
        const std::string &tableName, const std::vector<std::string> &columns)
        : modify_query(session) {
      tableName_ = tableName;
      columns_ = columns;
    }

    /*!
     * @param schema the schema to modify
     * @param column the specific columns to modify in the schema
     */
    insert_query::insert_query(const std::shared_ptr<schema> &schema,
                               const std::vector<std::string> &columns)
        : modify_query(schema->get_session()) {
      tableName_ = schema->table_name();
      columns_ = columns;
    }

    insert_query::insert_query(const insert_query &other)
        : modify_query(other), lastId_(other.lastId_), columns_(other.columns_),
          tableName_(other.tableName_) {}
    insert_query::insert_query(insert_query &&other)
        : modify_query(std::move(other)), lastId_(other.lastId_),
          columns_(std::move(other.columns_)),
          tableName_(std::move(other.tableName_)) {}

    insert_query::~insert_query() {}
    insert_query &insert_query::operator=(const insert_query &other) {
      modify_query::operator=(other);
      lastId_ = other.lastId_;
      columns_ = other.columns_;
      tableName_ = other.tableName_;
      return *this;
    }

    insert_query &insert_query::operator=(insert_query &&other) {
      modify_query::operator=(std::move(other));
      lastId_ = other.lastId_;
      columns_ = std::move(other.columns_);
      tableName_ = std::move(other.tableName_);
      return *this;
    }

    bool insert_query::is_valid() const noexcept {
      return query::is_valid() && !tableName_.empty();
    }

    long long insert_query::last_insert_id() const { return lastId_; }

    string insert_query::generate_sql() const {
      string buf = "INSERT INTO ";

      buf += tableName_;

      buf += "(";

      buf += coda::db::helper::join_csv(columns_);

      buf += ") VALUES(";

      buf += session_->join_params(columns_);

      buf += ")";

      if (session_->has_feature(session::FEATURE_RETURNING)) {
        auto schema = session_->get_schema(tableName_);

        if (schema != nullptr) {
          auto keys = schema->primary_keys();

          auto it = keys.begin();

          if (it != keys.end()) {
            buf += " RETURNING ";

            while (it < keys.end() - 1) {
              buf += *it;
              buf += ",";
            }

            buf += *it;
          }
        }
      }

      buf += ";";
      return buf;
    }

    insert_query &insert_query::columns(const vector<string> &columns) {
      columns_ = columns;
      set_modified();
      return *this;
    }

    vector<string> insert_query::columns() const { return columns_; }

    int insert_query::execute() {
      if (!is_valid()) {
        throw database_exception("invalid insert query");
      }

      prepare(to_sql());

      bool success = stmt_->result();

      if (success) {
        numChanges_ = stmt_->last_number_of_changes();
        lastId_ = stmt_->last_insert_id();
      } else {
        lastId_ = 0;
        numChanges_ = 0;
      }

      reset();

      return numChanges_;
    }

    insert_query &insert_query::into(const std::string &value) {
      tableName_ = value;
      set_modified();
      return *this;
    }

    std::string insert_query::into() const { return tableName_; }

    insert_query &insert_query::values(const std::vector<sql_value> &value) {
      bindable::bind(value);
      set_modified();
      return *this;
    }

    insert_query &insert_query::values(
        const std::unordered_map<std::string, sql_value> &value) {
      bindable::bind(value);
      set_modified();
      return *this;
    }

    insert_query &insert_query::value(const std::string &name,
                                      const sql_value &value) {
      bind(name, value);
      set_modified();
      return *this;
    }

    insert_query &insert_query::value(const sql_value &value) {
      bind(num_of_bindings() + 1, value);
      set_modified();
      return *this;
    }
  } // namespace db
} // namespace coda

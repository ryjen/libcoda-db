
#include "session.h"
#include <sstream>
#include "../schema.h"
#include "../select_query.h"
#include "../sqldb.h"
#include "resultset.h"
#include "statement.h"
#include "transaction.h"

using namespace std;

namespace coda::db::mysql {

  namespace helper {
    struct close_db {
      void operator()(MYSQL *p) const {
        if (p != nullptr) {
          mysql_close(p);
        }
      }
    };

    string last_stmt_error(MYSQL_STMT *stmt) {
      if (!stmt) {
        return "invalid";
      }

      ostringstream buf;

      buf << mysql_stmt_errno(stmt);
      buf << ": " << mysql_stmt_error(stmt);

      return buf.str();
    }
  }  // namespace helper

  __attribute__((constructor)) void initialize() {
    auto mysql_factory = std::make_shared<mysql::factory>();
    register_session("mysql", mysql_factory);
  }

  std::shared_ptr<coda::db::session_impl> factory::create(const uri &uri) { return std::make_shared<session>(uri); }

  session::session(const uri &connInfo) : session_impl(connInfo), db_(nullptr) {}

  session::~session() {
    if (is_open()) {
      close();
    }
  }

  void session::open() {
    if (db_ != nullptr) {
      return;
    }

    MYSQL *conn = mysql_init(nullptr);

    if (conn == nullptr) {
      throw database_exception("out of memory connecting to mysql");
    }

    auto info = connection_info();

    unsigned int port = 3306;

    try {
      if (!info.port.empty()) {
        port = static_cast<unsigned int>(std::stoi(info.port));
      }
    } catch (const std::exception &e) {
      mysql_close(conn);
      throw database_exception("unable to parse port " + info.port);
    }

    if (mysql_real_connect(conn, info.host.c_str(), info.user.c_str(),
                           info.password.empty() ? nullptr : info.password.c_str(), info.path.c_str(),
                           port, nullptr, 0) == nullptr) {
      string err = mysql_error(conn);
      mysql_close(conn);
      throw database_exception("No connection could be made to the database: " + err);
    }

    db_ = shared_ptr<MYSQL>(conn, helper::close_db());
  }

  bool session::is_open() const noexcept { return db_ != nullptr && db_; }

  void session::close() {
    if (db_ != nullptr) {
      db_ = nullptr;
    }
  }

  string session::last_error() const {
    if (db_ == nullptr) {
      return string();
    }

    ostringstream buf;

    buf << mysql_errno(db_.get());
    buf << ": " << mysql_error(db_.get());

    return buf.str();
  }

  long long session::last_insert_id() const {
    if (db_ == nullptr) {
      return 0;
    }

    return mysql_insert_id(db_.get());
  }

  unsigned long long session::last_number_of_changes() const {
    if (db_ == nullptr) {
      return 0;
    }

    return mysql_affected_rows(db_.get());
  }

  std::shared_ptr<resultset_impl> session::query(const string &sql) {
    MYSQL_RES *res = nullptr;

    if (db_ == nullptr) {
      throw database_exception("database is not open");
    }

    if (mysql_real_query(db_.get(), sql.c_str(), sql.length())) {
      throw database_exception(last_error());
    }

    res = mysql_store_result(db_.get());

    if (res == nullptr && mysql_field_count(db_.get()) != 0) {
      throw database_exception(last_error());
    }

    return make_shared<resultset>(shared_from_this(), shared_ptr<MYSQL_RES>(res, helper::res_delete()));
  }

  bool session::execute(const string &sql) {
    if (db_ == nullptr) {
      throw database_exception("database is not open");
    }

    return !mysql_real_query(db_.get(), sql.c_str(), sql.length());
  }

  shared_ptr<coda::db::session::statement_type> session::create_statement() {
    return make_shared<statement>(static_pointer_cast<mysql::session>(shared_from_this()));
  }

  std::shared_ptr<transaction_impl> session::create_transaction() const {
    return make_shared<mysql::transaction>(db_);
  }

  std::vector<std::string> session::get_primary_keys(const string &dbName, const string &tableName) {

    constexpr static const char* const pk_sql = "SELECT tc.table_schema, tc.table_name, kc.column_name "
                    "FROM information_schema.table_constraints tc "
                    "JOIN information_schema.key_column_usage kc ON kc.table_name = tc.table_name"
                    " AND kc.table_schema = tc.table_schema "
                    "WHERE tc.constraint_type = 'PRIMARY KEY' AND tc.table_name = ?"
                    " AND tc.table_schema = ? "
                    "ORDER BY tc.table_schema, tc.table_name,"
                    " kc.position_in_unique_constraint;";

    auto stmt = create_statement();

    stmt->prepare(pk_sql);
    stmt->bind(1, tableName);
    stmt->bind(2, dbName);

    std::vector<std::string> primary_keys;

    for (const auto &row : stmt->query()) {
      primary_keys.push_back(row["column_name"]);
    }

    return primary_keys;
  }

  std::vector<column_definition> session::get_columns_for_schema(const string &dbName, const string &tableName) {

    std::vector<column_definition> columns;

    if (!is_open()) {
      return columns;
    }

    constexpr static const char* const col_sql =
        "SELECT column_name, data_type, extra, column_default "
        "FROM information_schema.columns "
        "WHERE table_name = ? AND table_schema = ?;";

    auto primary_keys = get_primary_keys(dbName, tableName);

    auto stmt = create_statement();

    stmt->prepare(col_sql);
    stmt->bind(1, tableName);
    stmt->bind(2, dbName);


    for (const auto &row : stmt->query()) {
      column_definition def;

      // column name
      def.name = row["column_name"].as<std::string>();

      if (def.name.empty()) {
        continue;
      }

      def.pk = false;
      def.autoincrement = false;

      if (std::find(primary_keys.begin(), primary_keys.end(), def.name) != primary_keys.end()) {
          def.pk = true;
          def.autoincrement = row["extra"] == "auto_increment";
      }

      // find type
      def.type = row["data_type"].as<std::string>();

      def.default_value = row["column_default"].as<std::string>();

      columns.push_back(def);
    }

    return columns;
  }

  std::string session::bind_param(size_t) const { return "?"; }

  constexpr int session::features() const { return db::session::FEATURE_RIGHT_JOIN; }

}  // namespace coda::db::mysql

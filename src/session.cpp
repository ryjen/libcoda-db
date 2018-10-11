/*!
 * @copyright ryan jennings (coda.life), 2013
 */
#include "session.h"
#include "query.h"
#include "resultset.h"
#include "schema.h"
#include "select_query.h"
#include "sqlite/session.h"
#include "statement.h"

using namespace std;

namespace coda::db {

  session_impl::session_impl(const uri &info) : connectionInfo_(info) {}

  uri session_impl::connection_info() const { return connectionInfo_; }

  session::session(const std::shared_ptr<session_impl> &impl) : impl_(impl) {}

  uri session::connection_info() const { return impl_->connection_info(); }

  session::transaction_type session::start_transaction() {
    auto tx = create_transaction();
    tx.start();
    return tx;
  }

  std::shared_ptr<schema> session::get_schema(const std::string &tableName) {
    return schema_factory_.get(shared_from_this(), tableName);
  }

  void session::clear_schema(const std::string &tableName) { schema_factory_.clear(tableName); }

  bool session::is_open() const noexcept { return impl_->is_open(); }

  void session::open() { return impl_->open(); }

  void session::close() { return impl_->close(); }

  sql_id session::last_insert_id() const { return impl_->last_insert_id(); }

  sql_changes session::last_number_of_changes() const { return impl_->last_number_of_changes(); }

  std::shared_ptr<session::statement_type> session::create_statement(const std::string &sql) {
    auto stmt = impl_->create_statement();
    if (!sql.empty()) {
      stmt->prepare(sql);
    }
    return stmt;
  }

  session::transaction_type session::create_transaction() {
    return session::transaction_type(shared_from_this(), impl_->create_transaction());
  }

  std::string session::last_error() const { return impl_->last_error(); }

  std::vector<column_definition> session::get_columns_for_schema(const std::string &tablename) {
    return impl_->get_columns_for_schema(connection_info().path, tablename);
  }

  int session_impl::features() const { return 0; }

  shared_ptr<session_impl> session::impl() const { return impl_; }

  bool session::has_feature(feature_type feature) const { return (impl_->features() & feature) != 0; }

  /*!
   * utility method used in creating sql
   */
  string session::join_params(const vector<string> &columns, const std::string &op) const {
    ostringstream buf;

    for (string::size_type i = 0; i < columns.size(); i++) {
      if (!op.empty()) {
        buf << columns[i];
        buf << op;
      }

      buf << impl_->bind_param(i + 1);

      if (i + 1 < columns.size()) {
        buf.put(',');
      }
    }
    return buf.str();
  }
}  // namespace coda::db

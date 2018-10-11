/*!
 * @copyright ryan jennings (coda.life), 2013
 */
#include "modify_query.h"
#include "schema.h"
#include "statement.h"

using namespace std;

namespace coda::db {
  modify_query::modify_query(const std::shared_ptr<coda::db::session> &session)
      : query(session), flags_(0), numChanges_(0) {}

  modify_query::modify_query(const shared_ptr<schema> &schema) : modify_query(schema->get_session()) {}

  sql_changes modify_query::last_number_of_changes() const { return numChanges_; }

  sql_changes modify_query::execute() {
    if (!is_valid()) {
      throw database_exception("Invalid modify query");
    }

    prepare(to_sql());

    if (stmt_->execute()) {
      numChanges_ = stmt_->last_number_of_changes();
    } else {
      numChanges_ = 0;
    }

    reset();

    return numChanges_;
  }
}  // namespace coda::db

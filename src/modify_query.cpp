/*!
 * @copyright ryan jennings (ryan-jennings.net), 2013
 */
#include "modify_query.h"
#include "exception.h"
#include "log.h"
#include "schema.h"
#include "statement.h"

using namespace std;

namespace rj
{
    namespace db
    {
        modify_query::modify_query(const std::shared_ptr<rj::db::session> &session) : query(session), flags_(0), numChanges_(0)
        {
        }

        modify_query::modify_query(const shared_ptr<schema> &schema) : modify_query(schema->get_session())
        {
        }

        modify_query::modify_query(const modify_query &other) : query(other), flags_(other.flags_), numChanges_(other.numChanges_)
        {
        }
        modify_query::modify_query(modify_query &&other) : query(std::move(other)), flags_(other.flags_), numChanges_(other.numChanges_)
        {
        }

        modify_query::~modify_query()
        {
        }
        modify_query &modify_query::operator=(const modify_query &other)
        {
            query::operator=(other);
            flags_ = other.flags_;
            numChanges_ = other.numChanges_;
            return *this;
        }

        modify_query &modify_query::operator=(modify_query &&other)
        {
            query::operator=(std::move(other));
            flags_ = other.flags_;
            numChanges_ = other.numChanges_;
            return *this;
        }

        int modify_query::last_number_of_changes() const
        {
            return numChanges_;
        }

        int modify_query::execute()
        {
            if (!is_valid()) {
                throw database_exception("Invalid modify query");
            }

            prepare(to_string());

            bool success = stmt_->result();

            if (success) {
                numChanges_ = stmt_->last_number_of_changes();
            } else {
                log::error("%s", stmt_->last_error().c_str());
                numChanges_ = 0;
            }

            stmt_->reset();

            return numChanges_;
        }
    }
}

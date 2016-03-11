/*!
 * @copyright ryan jennings (arg3.com), 2013
 */
#include "modify_query.h"
#include "exception.h"
#include "schema.h"
#include "statement.h"
#include "log.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        modify_query::modify_query(const std::shared_ptr<arg3::db::session> &session) : query(session), flags_(0), numChanges_(0)
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

        modify_query &modify_query::flags(int value)
        {
            flags_ = value;
            return *this;
        }

        int modify_query::flags() const
        {
            return flags_;
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
                log::error(stmt_->last_error().c_str());
                numChanges_ = 0;
            }

            if (flags_ & Batch) {
                reset();
            } else {
                stmt_->finish();
                stmt_ = nullptr;
            }

            return numChanges_;
        }
    }
}

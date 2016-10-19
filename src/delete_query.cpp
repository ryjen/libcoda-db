#include "delete_query.h"
#include "log.h"
#include "schema.h"

using namespace std;

namespace rj
{
    namespace db
    {
        delete_query::delete_query(const std::shared_ptr<rj::db::session> &session) : modify_query(session), where_(session->impl(), this)
        {
        }

        delete_query::delete_query(const std::shared_ptr<rj::db::session> &session, const std::string &tableName)
            : modify_query(session), where_(session->impl(), this)
        {
            tableName_ = tableName;
        }

        delete_query::delete_query(const shared_ptr<schema> &schema) : modify_query(schema), where_(schema->get_session()->impl(), this)
        {
            tableName_ = schema->table_name();
        }

        delete_query::delete_query(const delete_query &other) : modify_query(other), where_(other.where_), tableName_(other.tableName_)
        {
        }
        delete_query::delete_query(delete_query &&other)
            : modify_query(std::move(other)), where_(std::move(other.where_)), tableName_(std::move(other.tableName_))
        {
        }

        delete_query::~delete_query()
        {
        }

        delete_query &delete_query::operator=(const delete_query &other)
        {
            modify_query::operator=(other);
            where_ = other.where_;
            tableName_ = other.tableName_;
            return *this;
        }

        delete_query &delete_query::operator=(delete_query &&other)
        {
            modify_query::operator=(std::move(other));
            where_ = std::move(other.where_);
            tableName_ = std::move(other.tableName_);
            return *this;
        }

        bool delete_query::is_valid() const
        {
            return query::is_valid() && !tableName_.empty();
        }

        where_builder &delete_query::where()
        {
            return where_;
        }

        where_builder &delete_query::where(const sql_operator &value)
        {
            where_.reset(value);
            return where_;
        }

        delete_query &delete_query::where(const where_clause &value)
        {
            where_.where_clause::reset(value);
            return *this;
        }

        where_clause &delete_query::where(const string &value)
        {
            where_.where_clause::reset(value);
            return where_;
        }

        delete_query &delete_query::from(const std::string &value)
        {
            tableName_ = value;
            return *this;
        }

        std::string delete_query::from() const
        {
            return tableName_;
        }

        string delete_query::to_string() const
        {
            string buf;

            buf += "DELETE FROM ";
            buf += tableName_;

            if (!where_.empty()) {
                buf += " WHERE ";
                buf += where_.to_string();
            } else {
                log::warn("empty where clause for delete query");
            }

            buf += ";";

            return buf;
        }
    }
}

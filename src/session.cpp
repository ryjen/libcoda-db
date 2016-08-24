/*!
 * @copyright ryan jennings (ryan-jennings.net), 2013
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <algorithm>
#include "exception.h"
#include "mysql/session.h"
#include "postgres/session.h"
#include "query.h"
#include "resultset.h"
#include "schema.h"
#include "select_query.h"
#include "session.h"
#include "sqlite/session.h"
#include "transaction.h"

using namespace std;

namespace rj
{
    namespace db
    {
        session_impl::session_impl(const uri &info) : connectionInfo_(info)
        {
        }

        uri session_impl::connection_info() const
        {
            return connectionInfo_;
        }

        session::session(const std::shared_ptr<session_impl> &impl) : impl_(impl)
        {
        }

        session::session(const session &other) : impl_(other.impl_), schema_factory_(other.schema_factory_)
        {
        }

        session::session(session &&other) : impl_(std::move(other.impl_)), schema_factory_(std::move(other.schema_factory_))
        {
            other.impl_ = nullptr;
        }

        session &session::operator=(const session &other)
        {
            impl_ = other.impl_;
            schema_factory_ = other.schema_factory_;
            return *this;
        }
        session &session::operator=(session &&other)
        {
            impl_ = std::move(other.impl_);
            schema_factory_ = std::move(other.schema_factory_);
            other.impl_ = nullptr;
            return *this;
        }
        session::~session()
        {
            // shared_ptr's will handle impl closing
        }

        uri session::connection_info() const
        {
            return impl_->connection_info();
        }

        session::transaction_type session::start_transaction()
        {
            auto tx = create_transaction();
            tx.start();
            return tx;
        }

        std::shared_ptr<schema> session::get_schema(const std::string &tableName)
        {
            return schema_factory_.get(shared_from_this(), tableName);
        }

        void session::clear_schema(const std::string &tableName)
        {
            schema_factory_.clear(tableName);
        }

        bool session::is_open() const
        {
            return impl_->is_open();
        }

        void session::open()
        {
            return impl_->open();
        }

        void session::close()
        {
            return impl_->close();
        }

        long long session::last_insert_id() const
        {
            return impl_->last_insert_id();
        }

        int session::last_number_of_changes() const
        {
            return impl_->last_number_of_changes();
        }

        session::resultset_type session::query(const std::string &sql) const
        {
            return resultset_type(impl_->query(sql));
        }

        bool session::execute(const std::string &sql)
        {
            return impl_->execute(sql);
        }

        std::shared_ptr<session::statement_type> session::create_statement()
        {
            return impl_->create_statement();
        }

        session::transaction_type session::create_transaction()
        {
            return session::transaction_type(shared_from_this(), impl_->create_transaction());
        }

        std::string session::last_error() const
        {
            return impl_->last_error();
        }

        std::vector<column_definition> session::get_columns_for_schema(const std::string &tablename)
        {
            return impl_->get_columns_for_schema(connection_info().path, tablename);
        }

        string session_impl::get_insert_sql(const std::shared_ptr<schema> &schema, const vector<string> &columns) const
        {
            string buf;

            buf += "INSERT INTO ";
            buf += schema->table_name();

            buf += "(";

            buf += helper::join_csv(columns);

            buf += ") VALUES(";

            buf += helper::join_params(columns, false);

            buf += ");";

            return buf;
        }

        string session::get_insert_sql(const std::shared_ptr<schema> &schema, const vector<string> &columns) const
        {
            return impl_->get_insert_sql(schema, columns);
        }

        shared_ptr<session_impl> session::impl() const
        {
            return impl_;
        }
    }
}

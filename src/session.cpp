/*!
 * @copyright ryan jennings (arg3.com), 2013
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <algorithm>
#include "session.h"
#include "query.h"
#include "exception.h"
#include "resultset.h"
#include "sqlite/session.h"
#include "mysql/session.h"
#include "postgres/session.h"
#include "select_query.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        session::session()
        {
        }

        session::session(const uri &connectionInfo) : connectionInfo_(connectionInfo)
        {
        }


        uri session::connection_info() const
        {
            return connectionInfo_;
        }

        void session::set_connection_info(const uri &value)
        {
            connectionInfo_ = value;
        }


        std::shared_ptr<schema> session::get_schema(const std::string &tableName)
        {
            return schema_factory_.get(shared_from_this(), tableName);
        }

        void session::clear_schema(const std::string &tableName)
        {
            schema_factory_.clear(tableName);
        }

        string session::insert_sql(const std::shared_ptr<schema> &schema, const vector<string> &columns) const
        {
            ostringstream buf;

            buf << "INSERT INTO " << schema->table_name();

            buf << "(";

            buf << join_csv(columns);

            buf << ") VALUES(";

            buf << join_params(columns, false);

            buf << ");";

            return buf.str();
        }
    }
}

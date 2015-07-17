/*!
 * implementation of a query
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "base_query.h"
#include "exception.h"
#include "sqldb.h"
#include <cassert>

namespace std
{
    string to_string(const arg3::db::base_query &query)
    {
        return query.to_string();
    }
}
namespace arg3
{
    namespace db
    {

        base_query::base_query(sqldb *db, const string &tableName) : db_(db), stmt_(nullptr), tableName_(tableName)
        {}

        base_query::base_query(shared_ptr<schema> schema) : db_(schema->db()), stmt_(nullptr), tableName_(schema->table_name())
        {}

        base_query::base_query(const base_query &other) : db_(other.db_), stmt_(other.stmt_),
            tableName_(other.tableName_), bindings_(other.bindings_)
        {}

        base_query::base_query(base_query &&other) : db_(other.db_), stmt_(std::move(other.stmt_)),
            tableName_(std::move(other.tableName_)), bindings_(std::move(other.bindings_))
        {
            other.db_ = NULL;
            other.stmt_ = nullptr;
        }

        base_query::~base_query() {}

        base_query &base_query::operator=(const base_query &other)
        {
            db_ = other.db_;
            stmt_ = other.stmt_;
            tableName_ = other.tableName_;
            bindings_ = other.bindings_;

            return *this;
        }

        base_query &base_query::operator=(base_query && other)
        {
            db_ = other.db_;
            stmt_ = std::move(other.stmt_);
            other.db_ = NULL;
            other.stmt_ = nullptr;
            tableName_ = std::move(other.tableName_);
            bindings_ = std::move(other.bindings_);

            return *this;
        }

        sqldb *base_query::db() const
        {
            return db_;
        }

        std::string base_query::tablename() const
        {
            return tableName_;
        }

        void base_query::prepare(const string &sql)
        {
            assert(db_ != NULL);

            if (stmt_ != nullptr && stmt_->is_valid()) return;

            db_->log(sqldb::LOG_VERBOSE, sql);

            if (stmt_ == nullptr)
                stmt_ = db_->create_statement();

            stmt_->prepare(sql);

            for (size_t i = 1; i <= bindings_.size(); i++)
            {
                auto &value = bindings_[i - 1];

                value.bind_to(stmt_.get(), i);
            }
        }

        void base_query::prepare()
        {
            prepare(to_string());
        }

        size_t base_query::assert_binding_index(size_t index)
        {
            assert(index > 0);

            if (index > bindings_.size())
            {
                bindings_.resize(index);
            }

            return index - 1;
        }

        base_query &base_query::bind(size_t index, const string &value, int len)
        {
            bindings_[assert_binding_index(index)] = len > 0 ? value.substr(0, len) : value;

            return *this;
        }

        base_query &base_query::bind(size_t index, int value)
        {
            bindings_[assert_binding_index(index)] = value;

            return *this;
        }

        base_query &base_query::bind(size_t index, long long value)
        {
            bindings_[assert_binding_index(index)] = value;

            return *this;
        }

        base_query &base_query::bind(size_t index)
        {
            bindings_[assert_binding_index(index)]  = nullptr;

            return *this;
        }

        base_query &base_query::bind(size_t index, const sql_null_type &value)
        {
            return bind(index);
        }

        base_query &base_query::bind(size_t index, double value)
        {
            bindings_[assert_binding_index(index)] = value;

            return *this;
        }

        base_query &base_query::bind(size_t index, const void *data, size_t size, void (*pFree)(void *))
        {
            bindings_[assert_binding_index(index)] = sql_blob(data, size, pFree);

            return *this;
        }


        base_query &base_query::bind(size_t index, const sql_blob &value)
        {
            bindings_[assert_binding_index(index)] = value;

            return *this;
        }

        base_query &base_query::bind_value(size_t index, const sql_value &value)
        {
            value.bind_to(this, index);

            return *this;
        }


        int base_query::last_number_of_changes()
        {
            if (stmt_ == nullptr) return 0;

            return stmt_->last_number_of_changes();
        }

        string base_query::last_error()
        {
            if ( stmt_ == nullptr) return string();

            return stmt_->last_error();
        }

        bool base_query::is_valid() const
        {
            return db_ != NULL;
        }

        void base_query::reset()
        {
            bindings_.clear();
            stmt_ = nullptr;
        }
    }


    string join_csv(string::value_type value, string::size_type count)
    {
        ostringstream buf;

        if (count > 0)
        {
            for (string::size_type i = 0; i < count - 1; i++)
            {
                buf.put(value);
                buf.put(',');
            }

            buf.put(value);
        }
        return buf.str();
    }
}
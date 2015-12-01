/*!
 * implementation of a query
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "query.h"
#include "exception.h"
#include "sqldb.h"
#include <cassert>

namespace arg3
{
    namespace db
    {
        query::query(sqldb *db) : db_(db), stmt_(nullptr)
        {
        }
        query::query(const query &other) : db_(other.db_), stmt_(other.stmt_), bindings_(other.bindings_)
        {
        }
        query::query(query &&other) : db_(other.db_), stmt_(std::move(other.stmt_)), bindings_(std::move(other.bindings_))
        {
            other.db_ = NULL;
            other.stmt_ = nullptr;
        }

        query::~query()
        {
        }
        query &query::operator=(const query &other)
        {
            db_ = other.db_;
            stmt_ = other.stmt_;
            bindings_ = other.bindings_;

            return *this;
        }

        query &query::operator=(query &&other)
        {
            db_ = other.db_;
            stmt_ = std::move(other.stmt_);
            other.db_ = NULL;
            other.stmt_ = nullptr;
            bindings_ = std::move(other.bindings_);

            return *this;
        }

        sqldb *query::db() const
        {
            return db_;
        }
        void query::prepare(const string &sql)
        {
            assert(db_ != NULL);

            if (stmt_ != nullptr && stmt_->is_valid()) return;

            db_->log(sqldb::LOG_VERBOSE, sql);

            if (stmt_ == nullptr) {
                stmt_ = db_->create_statement();
            }

            stmt_->prepare(sql);

            for (size_t i = 1; i <= bindings_.size(); i++) {
                auto &value = bindings_[i - 1];

                value.bind_to(stmt_.get(), i);
            }
        }

        size_t query::assert_binding_index(size_t index)
        {
            assert(index > 0);

            if (index > bindings_.size()) {
                bindings_.resize(index);
            }

            return index - 1;
        }

        query &query::bind(size_t index, const string &value, int len)
        {
            bindings_[assert_binding_index(index)] = len > 0 ? value.substr(0, len) : value;

            return *this;
        }
        query &query::bind(size_t index, const wstring &value, int len)
        {
            bindings_[assert_binding_index(index)] = len > 0 ? value.substr(0, len) : value;

            return *this;
        }
        query &query::bind(size_t index, int value)
        {
            bindings_[assert_binding_index(index)] = value;

            return *this;
        }

        query &query::bind(size_t index, long long value)
        {
            bindings_[assert_binding_index(index)] = value;

            return *this;
        }

        query &query::bind(size_t index)
        {
            bindings_[assert_binding_index(index)] = nullptr;

            return *this;
        }

        query &query::bind(size_t index, const sql_null_t &value)
        {
            return bind(index);
        }
        query &query::bind(size_t index, double value)
        {
            bindings_[assert_binding_index(index)] = value;

            return *this;
        }

        query &query::bind(size_t index, const sql_blob &value)
        {
            bindings_[assert_binding_index(index)] = value;

            return *this;
        }

        query &query::bind_value(size_t index, const sql_value &value)
        {
            value.bind_to(this, index);

            return *this;
        }

        int query::last_number_of_changes()
        {
            if (stmt_ == nullptr) return 0;

            return stmt_->last_number_of_changes();
        }

        string query::last_error()
        {
            if (stmt_ == nullptr) return string();

            return stmt_->last_error();
        }

        bool query::is_valid() const
        {
            return db_ != NULL;
        }
        void query::reset()
        {
            bindings_.clear();
            stmt_ = nullptr;
        }
    }

    string join_csv(string::value_type value, string::size_type count)
    {
        ostringstream buf;

        if (count > 0) {
            for (string::size_type i = 0; i < count - 1; i++) {
                buf.put(value);
                buf.put(',');
            }

            buf.put(value);
        }
        return buf.str();
    }
}
/*!
 * implementation of a query
 * @copyright ryan jennings (arg3.com), 2013
 */
#include "query.h"
#include "exception.h"
#include "sqldb.h"
#include "log.h"
#include <cassert>

using namespace std;

namespace arg3
{
    namespace db
    {
        query::query(sqldb *db) : db_(db), stmt_(nullptr), bindings_()
        {
            if (db_ == nullptr) {
                throw database_exception("No database provided for query");
            }
        }

        query::query(const query &other) : db_(other.db_), stmt_(other.stmt_), bindings_(other.bindings_)
        {
        }

        query::query(query &&other) : db_(other.db_), stmt_(std::move(other.stmt_)), bindings_(std::move(other.bindings_))
        {
            other.db_ = nullptr;
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
            bindings_ = std::move(other.bindings_);
            other.db_ = nullptr;
            other.stmt_ = nullptr;

            return *this;
        }

        sqldb *query::db() const
        {
            return db_;
        }

        void query::prepare(const string &sql)
        {
            if (stmt_ != nullptr) {
                // check if the statement is already prepared
                if (stmt_->is_valid()) {
                    return;
                }
            } else {
                stmt_ = db_->create_statement();
            }

            log::trace("Query: %s", sql.c_str());

            stmt_->prepare(sql);  // throws exception on error

            for (size_t i = 1; i <= bindings_.size(); i++) {
                auto &value = bindings_[i - 1];

                stmt_->bind_value(i, value);
            }
        }

        size_t query::assert_binding_index(size_t index)
        {
            if (index == 0) {
                throw binding_error("parameter index must be greater than zero");
            }

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
        query &query::bind(size_t index, unsigned value)
        {
            bindings_[assert_binding_index(index)] = value;

            return *this;
        }

        query &query::bind(size_t index, long long value)
        {
            bindings_[assert_binding_index(index)] = value;

            return *this;
        }
        query &query::bind(size_t index, unsigned long long value)
        {
            bindings_[assert_binding_index(index)] = value;

            return *this;
        }

        query &query::bind(size_t index)
        {
            bindings_[assert_binding_index(index)] = nullptr;

            return *this;
        }

        query &query::bind(size_t index, const sql_null_type &value)
        {
            return bind(index);
        }
        query &query::bind(size_t index, float value)
        {
            bindings_[assert_binding_index(index)] = value;

            return *this;
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
        query &query::bind(size_t index, const sql_time &value)
        {
            bindings_[assert_binding_index(index)] = value;

            return *this;
        }

        string query::last_error()
        {
            if (stmt_ == nullptr) return string();

            return stmt_->last_error();
        }

        bool query::is_valid() const
        {
            return db_ != nullptr;
        }
        void query::reset()
        {
            bindings_.clear();
            stmt_ = nullptr;
        }
    }

    string join_params(const vector<string> &columns, bool update)
    {
        ostringstream buf;

        for (string::size_type i = 0; i < columns.size(); i++) {
            if (update) {
                buf << columns[i];
                buf << " = ";
            }

            buf.put('$');
            buf << (i + 1);

            if (i + 1 < columns.size()) {
                buf.put(',');
            }
        }
        return buf.str();
    }
}

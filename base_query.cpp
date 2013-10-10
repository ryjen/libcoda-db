/*!
 * implementation of a query
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "base_query.h"
#include "exception.h"
#include "sqldb.h"
#include <cassert>

namespace arg3
{
    namespace db
    {
        class query_binding_visitor
        {
            base_query *query_;
            int index_;
        public:
            typedef void result_type;

            query_binding_visitor(base_query *query, int index) : query_(query), index_(index)
            {}

            template<typename T>
            void operator()(T value) const {
                query_->bind(index_, value);
            }
        };

        class sql_binding_visitor
        {
            sqldb *db_;
            sqlite3_stmt *stmt_;
            int index_;
        public:
            typedef void result_type;

            sql_binding_visitor(sqldb *db, sqlite3_stmt *stmt, int index) : db_(db), stmt_(stmt), index_(index)
            {}

            void operator()(int value) const {
                if(sqlite3_bind_int(stmt_, index_, value) != SQLITE_OK)
                    throw binding_error(db_->last_error());
            }
            void operator()(int64_t value) const {
                if(sqlite3_bind_int64(stmt_, index_, value) != SQLITE_OK)
                    throw binding_error(db_->last_error());
            }
            void operator()(double value) const {
                if(sqlite3_bind_double(stmt_, index_, value) != SQLITE_OK)
                    throw binding_error(db_->last_error());
            }
            void operator()(std::string value) const {
                if(sqlite3_bind_text(stmt_, index_, value.c_str(), value.size(), SQLITE_TRANSIENT) != SQLITE_OK)
                    throw binding_error(db_->last_error());
            }
            void operator()(sql_blob value) const {
                if(sqlite3_bind_blob(stmt_, index_, value.ptr(), value.size(), value.destructor()) != SQLITE_OK)
                    throw binding_error(db_->last_error());
            }
            void operator()(sql_null_t value) const {
                if(sqlite3_bind_null(stmt_, index_) != SQLITE_OK)
                    throw binding_error(db_->last_error());
            }
        };

        base_query::base_query(sqldb *db, const string &tableName) : db_(db), stmt_(NULL), tableName_(tableName)
        {}

        base_query::base_query(const base_query &other) : db_(other.db_), stmt_(other.stmt_),
            tableName_(other.tableName_), bindings_(other.bindings_)
        {}

        base_query::base_query(base_query &&other) : db_(std::move(other.db_)), stmt_(std::move(other.stmt_)),
            tableName_(std::move(other.tableName_)), bindings_(std::move(other.bindings_))
        {
            other.db_ = NULL;
            other.stmt_ = NULL;
        }

        base_query::~base_query() {}

        base_query &base_query::operator=(const base_query &other)
        {
            if(this != &other)
            {
                db_ = other.db_;
                stmt_ = other.stmt_;
                tableName_ = other.tableName_;
                bindings_ = other.bindings_;
            }
            return *this;
        }

        base_query &base_query::operator=(base_query &&other)
        {
            if(this != &other)
            {
                db_ = other.db_;
                stmt_ = other.stmt_;
                other.db_ = NULL;
                other.stmt_ = NULL;
                tableName_ = std::move(other.tableName_);
                bindings_ = std::move(other.bindings_);
            }
            return *this;
        }

        void base_query::prepare()
        {
            assert(db_ != NULL);

            if (stmt_ != NULL) return;

            string sql = to_string();

            if (sqlite3_prepare_v2(db_->db_, sql.c_str(), -1, &stmt_, NULL) != SQLITE_OK)
                throw database_exception(db_->last_error());

            for(size_t i = 1; i <= bindings_.size(); i++)
            {
                auto value = bindings_[i-1];

                apply_visitor(sql_binding_visitor(db_, stmt_, i), value);
            }
        }

        size_t base_query::assert_binding_index(size_t index)
        {
            assert(index > 0);

            bindings_.resize(std::max(index, bindings_.size()));

            return index-1;
        }

        base_query &base_query::bind(size_t index, const string &value, int len)
        {
            bindings_[assert_binding_index(index)] = sql_value(len > 0 ? value.substr(0, len) : value);

            return *this;
        }

        base_query &base_query::bind(size_t index, int value)
        {
            bindings_[assert_binding_index(index)] = sql_value(value);

            return *this;
        }

        base_query &base_query::bind(size_t index, long long value)
        {
            bindings_[assert_binding_index(index)] = sql_value(value);

            return *this;
        }

        base_query &base_query::bind(size_t index)
        {
            bindings_[assert_binding_index(index)]  = sql_null;

            return *this;
        }

        base_query &base_query::bind(size_t index, const sql_null_t &value)
        {
            return bind(index);
        }

        base_query &base_query::bind(size_t index, double value)
        {
            bindings_[assert_binding_index(index)] = sql_value(value);

            return *this;
        }

        base_query &base_query::bind(size_t index, const void *data, size_t size, void (*pFree)(void*))
        {
            bindings_[assert_binding_index(index)] = sql_value(sql_blob(data, size, pFree));

            return *this;
        }


        base_query &base_query::bind(size_t index, const sql_blob &value)
        {
            bindings_[assert_binding_index(index)] = sql_value(value);

            return *this;
        }

        base_query &base_query::bind_value(size_t index, const sql_value &value) {

            apply_visitor(query_binding_visitor(this, index), value);

            return *this;
        }
    }


    string join(string::value_type value, string::size_type count, const string &delimiter)
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
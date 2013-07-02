/*!
 * implementation of a query
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "base_query.h"
#include "exception.h"
#include "sqldb.h"
#include <cassert>
#include "../variant/variant.h"

namespace arg3
{
    namespace db
    {
        base_query::base_query(sqldb *db, const string &tableName) : db_(db), stmt_(NULL), tableName_(tableName)
        {}

        base_query::base_query(const base_query &other) : db_(other.db_), stmt_(other.stmt_),
            tableName_(other.tableName_), bindings_(other.bindings_)
        {}

        base_query::base_query(base_query &&other) : db_(std::move(other.db_)), stmt_(std::move(other.stmt_)),
            tableName_(std::move(other.tableName_)), bindings_(std::move(other.bindings_))
        {}

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
                db_ = std::move(other.db_);
                stmt_ = std::move(other.stmt_);
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
                throw database_exception(db_->lastError());

            for(size_t i = 1; i <= bindings_.size(); i++)
            {
                auto b = bindings_[i-1];

                switch(b.type)
                {
                case SQLITE_TEXT:
                {
                    if (sqlite3_bind_text(stmt_, i, b.value.to_cstring(), b.size, SQLITE_TRANSIENT) != SQLITE_OK)
                        throw database_exception(db_->lastError());
                    break;
                }
                case SQLITE_INTEGER:
                    if(b.size > sizeof(int))
                    {
                        if (sqlite3_bind_int64(stmt_, i, b.value.to_llong()) != SQLITE_OK)
                            throw database_exception(db_->lastError());
                    }
                    else
                    {
                        if (sqlite3_bind_int(stmt_, i, b.value.to_int()) != SQLITE_OK)
                            throw database_exception(db_->lastError());
                    }
                    break;
                case SQLITE_FLOAT:
                    if (sqlite3_bind_double(stmt_, i, b.value.to_double()) != SQLITE_OK)
                        throw database_exception(db_->lastError());
                    break;
                case SQLITE_BLOB:
                    // TODO: add free method handling
                    if (sqlite3_bind_blob(stmt_, i, b.value.to_pointer(), b.size, b.freeFunc) != SQLITE_OK)
                        throw database_exception(db_->lastError());
                    break;
                default:
                    if(sqlite3_bind_null(stmt_, i) != SQLITE_OK)
                        throw database_exception(db_->lastError());
                    break;
                }
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
            bindings_[assert_binding_index(index)] = bind_type(value, len);

            return *this;
        }

        base_query &base_query::bind(size_t index, int value)
        {
            bindings_[assert_binding_index(index)] = bind_type(value);

            return *this;
        }

        base_query &base_query::bind(size_t index, long long value)
        {
            bindings_[assert_binding_index(index)] = bind_type(value);

            return *this;
        }

        base_query &base_query::bind(size_t index)
        {
            bindings_[assert_binding_index(index)]  = bind_type();

            return *this;
        }

        base_query &base_query::bind(size_t index, double value)
        {
            bindings_[assert_binding_index(index)] = bind_type(value);

            return *this;
        }

        base_query &base_query::bind(size_t index, const void *data, size_t size, void (*pFree)(void *))
        {
            bindings_[assert_binding_index(index)] = bind_type(data, size, pFree);

            return *this;
        }

        base_query &base_query::bind(size_t index, int type, const variant &value) {

            if(value.is_null())
            {
                bind(index);
            }
            else
            {
                switch (type)
                {
                case SQLITE_TEXT:
                    bind(index, value.to_string());
                    break;
                case SQLITE_INTEGER:
                    bind(index, value.to_llong());
                    break;
                case SQLITE_FLOAT:
                    bind(index, value.to_double());
                    break;
                case SQLITE_BLOB:
                    bind(index, value.to_pointer(), value.size());
                default:
                    bind(index);
                    break;
                }
            }
            return *this;
        }
    }
}
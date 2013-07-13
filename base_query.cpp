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
        base_query::base_query(sqldb *db, const string &tableName) : db_(db), stmt_(NULL), tableName_(tableName)
        {}

        base_query::base_query(const base_query &other) : db_(other.db_), stmt_(other.stmt_),
            tableName_(other.tableName_), bindings_(other.bindings_)
        {}

        base_query::base_query(base_query &&other) : db_(other.db_), stmt_(other.stmt_),
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
                throw database_exception(db_->lastError());

            for(size_t i = 1; i <= bindings_.size(); i++)
            {
                auto b = bindings_[i-1];

                if(b.type() == typeid(long)) {
                    if (sqlite3_bind_int(stmt_, i, boost::get<long>(b)) != SQLITE_OK)
                            throw database_exception(db_->lastError());
                }
                else if(b.type() == typeid(int64_t)) {
                    if (sqlite3_bind_int64(stmt_, i, boost::get<int64_t>(b)) != SQLITE_OK)
                            throw database_exception(db_->lastError());
                }
                else if(b.type() == typeid(std::string)) {
                     std::string temp = boost::get<std::string>(b);
                     if (sqlite3_bind_text(stmt_, i, boost::get<std::string>, b.size, SQLITE_TRANSIENT) != SQLITE_OK)
                        throw database_exception(db_->lastError());
                }
                else if(b.type() == typeid(double)) {
                    if (sqlite3_bind_double(stmt_, i, b.value.to_double()) != SQLITE_OK)
                        throw database_exception(db_->lastError());
                }
                else if(b.type() == typeid(sql_blob)) {

                    // TODO: add free method handling
                    if (sqlite3_bind_blob(stmt_, i, b.value.to_pointer(), b.size, b.freeFunc) != SQLITE_OK)
                        throw database_exception(db_->lastError());
                }
                else {

                    if(sqlite3_bind_null(stmt_, i) != SQLITE_OK)
                        throw database_exception(db_->lastError());
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
            bindings_[assert_binding_index(index)]  = sql_value();

            return *this;
        }

        base_query &base_query::bind(size_t index, double value)
        {
            bindings_[assert_binding_index(index)] = sql_value(value);

            return *this;
        }

        base_query &base_query::bind(size_t index, const void *data, size_t size, void (*pFree)(void *))
        {
            bindings_[assert_binding_index(index)] = sql_value(sql_blob(data, size, pFree));

            return *this;
        }


        base_query &base_query::bind(size_t index, const sql_blob &value)
        {
            bindings_[assert_binding_index(index)] = sql_value(value);

            return *this;
        }

        base_query &base_query::bind(size_t index, int type, const sql_value &value) {

            if(value.type() == typeid(std::string)) {
                bind(index, value.get<std::string>());
            }
            else if(value.type() == typeid(int64_t)) {
                bind(index, value.get<int64_t>());
            }
            else if(value.type() == typeid(double)) {
                bind(index, value.get<double>());
            }
            else if(value.type() == typeid(sql_blob)) {
                bind(index, value)
            }
            else if(value.type() == typeid(long)) {
                bind(index, value.get<long>());
            }
            else
            {
                bind(index);
            }
            return *this;
        }
    }
}
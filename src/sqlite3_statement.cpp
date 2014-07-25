#include "sqlite3_statement.h"
#include "sqlite3_db.h"
#include "sqlite3_resultset.h"

namespace arg3
{
    namespace db
    {

        sqlite3_statement::sqlite3_statement(sqlite3_db *db) : db_(db), stmt_(NULL)
        {}

        void sqlite3_statement::prepare(const string &sql)
        {
            if (stmt_ != NULL) return;

            if (sqlite3_prepare_v2(db_->db_, sql.c_str(), -1, &stmt_, NULL) != SQLITE_OK)
                throw database_exception(db_->last_error());
        }

        bool sqlite3_statement::is_valid() const
        {
            return stmt_ != NULL;
        }

        int sqlite3_statement::last_number_of_changes()
        {
            return db_->last_number_of_changes();
        }

        string sqlite3_statement::last_error()
        {
            return db_->last_error();
        }

        sqlite3_statement &sqlite3_statement::bind(size_t index, int value)
        {
            if (sqlite3_bind_int(stmt_, index, value) != SQLITE_OK)
                throw binding_error(db_->last_error());
            return *this;
        }
        sqlite3_statement &sqlite3_statement::bind(size_t index, int64_t value)
        {
            if (sqlite3_bind_int64(stmt_, index, value) != SQLITE_OK)
                throw binding_error(db_->last_error());
            return *this;
        }
        sqlite3_statement &sqlite3_statement::bind(size_t index, double value)
        {
            if (sqlite3_bind_double(stmt_, index, value) != SQLITE_OK)
                throw binding_error(db_->last_error());
            return *this;
        }
        sqlite3_statement &sqlite3_statement::bind(size_t index, const std::string &value, int len)
        {
            if (sqlite3_bind_text(stmt_, index, value.c_str(), len, SQLITE_TRANSIENT) != SQLITE_OK)
                throw binding_error(db_->last_error());
            return *this;
        }
        sqlite3_statement &sqlite3_statement::bind(size_t index, const sql_blob &value)
        {
            if (sqlite3_bind_blob(stmt_, index, value.ptr(), value.size(), value.destructor() != NULL ? SQLITE_TRANSIENT : SQLITE_STATIC) != SQLITE_OK)
                throw binding_error(db_->last_error());
            return *this;
        }
        sqlite3_statement &sqlite3_statement::bind(size_t index, const sql_null_type &value)
        {
            if (sqlite3_bind_null(stmt_, index) != SQLITE_OK)
                throw binding_error(db_->last_error());
            return *this;
        }

        sqlite3_statement &sqlite3_statement::bind(size_t index, const void *data, size_t size, void (*pFree)(void *))
        {
            if (sqlite3_bind_blob(stmt_, index, data, size, pFree) != SQLITE_OK)
                throw binding_error(db_->last_error());
            return *this;
        }

        sqlite3_statement &sqlite3_statement::bind_value(size_t index, const sql_value &value)
        {
            value.bind_to(this, index);
            return *this;
        }

        resultset sqlite3_statement::results()
        {
            return resultset(make_shared<sqlite3_resultset>(db_, stmt_));
        }

        bool sqlite3_statement::result()
        {
            return sqlite3_step(stmt_) == SQLITE_DONE;
        }

        void sqlite3_statement::finish()
        {
            if (sqlite3_finalize(stmt_) != SQLITE_OK)
                throw database_exception(db_->last_error());

            stmt_ = NULL;
        }

        void sqlite3_statement::reset()
        {
            if (sqlite3_reset(stmt_) != SQLITE_OK)
                throw database_exception(db_->last_error());
        }

        long long sqlite3_statement::last_insert_id()
        {
            return db_->last_insert_id();
        }
    }
}
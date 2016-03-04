#include "statement.h"
#include "db.h"
#include "resultset.h"
#include "../log.h"

#ifdef HAVE_LIBSQLITE3

using namespace std;

namespace arg3
{
    namespace db
    {
        namespace sqlite
        {
            namespace helper
            {
                void stmt_delete::operator()(sqlite3_stmt *p) const
                {
                    if (p != NULL) {
                        sqlite3_finalize(p);
                    }
                }
            }

            statement::statement(sqlite::db *db) : db_(db), stmt_(nullptr)
            {
                if (db_ == NULL) {
                    throw database_exception("no database provided to sqlite3 statement");
                }
            }

            statement::statement(statement &&other) : db_(other.db_), stmt_(other.stmt_)
            {
                other.stmt_ = nullptr;
                other.db_ = NULL;
            }

            statement &statement::operator=(statement &&other)
            {
                db_ = other.db_;
                stmt_ = other.stmt_;

                other.stmt_ = nullptr;
                other.db_ = NULL;

                return *this;
            }

            statement::~statement()
            {
            }

            void statement::prepare(const string &sql)
            {
                if (!db_ || !db_->db_) {
                    throw database_exception("database not open");
                }

                if (is_valid()) {
                    log::warn("sql statement prepare not valid");
                    return;
                }

                sqlite3_stmt *temp;

                if (sqlite3_prepare_v2(db_->db_.get(), sql.c_str(), -1, &temp, NULL) != SQLITE_OK) {
                    throw database_exception(db_->last_error());
                }

                stmt_ = shared_ptr<sqlite3_stmt>(temp, helper::stmt_delete());
            }

            bool statement::is_valid() const
            {
                return stmt_ != nullptr && stmt_;
            }

            int statement::last_number_of_changes()
            {
                return db_->last_number_of_changes();
            }

            string statement::last_error()
            {
                return db_->last_error();
            }

            statement &statement::bind(size_t index, int value)
            {
                if (!is_valid()) {
                    throw binding_error("statment invalid");
                }
                if (sqlite3_bind_int(stmt_.get(), index, value) != SQLITE_OK) {
                    throw binding_error(db_->last_error());
                }
                return *this;
            }
            statement &statement::bind(size_t index, unsigned value)
            {
                if (!is_valid()) {
                    throw binding_error("statment invalid");
                }
                if (sqlite3_bind_int64(stmt_.get(), index, value) != SQLITE_OK) {
                    throw binding_error(db_->last_error());
                }
                return *this;
            }
            statement &statement::bind(size_t index, long long value)
            {
                if (!is_valid()) {
                    throw binding_error("statment invalid");
                }
                if (sqlite3_bind_int64(stmt_.get(), index, value) != SQLITE_OK) {
                    throw binding_error(db_->last_error());
                }
                return *this;
            }
            statement &statement::bind(size_t index, unsigned long long value)
            {
                throw binding_error("sqlite3 does not support unsigned 64 bit integers");
            }
            statement &statement::bind(size_t index, float value)
            {
                if (!is_valid()) {
                    throw binding_error("statment invalid");
                }
                if (sqlite3_bind_double(stmt_.get(), index, value) != SQLITE_OK) {
                    throw binding_error(db_->last_error());
                }
                return *this;
            }
            statement &statement::bind(size_t index, double value)
            {
                if (!is_valid()) {
                    throw binding_error("statment invalid");
                }
                if (sqlite3_bind_double(stmt_.get(), index, value) != SQLITE_OK) {
                    throw binding_error(db_->last_error());
                }
                return *this;
            }
            statement &statement::bind(size_t index, const std::string &value, int len)
            {
                if (!is_valid()) {
                    throw binding_error("statment invalid");
                }
                if (sqlite3_bind_text(stmt_.get(), index, value.c_str(), len, SQLITE_TRANSIENT) != SQLITE_OK) {
                    throw binding_error(db_->last_error());
                }
                return *this;
            }
            statement &statement::bind(size_t index, const std::wstring &value, int len)
            {
                if (!is_valid()) {
                    throw binding_error("statment invalid");
                }
                if (sqlite3_bind_text16(stmt_.get(), index, value.c_str(), len, SQLITE_TRANSIENT) != SQLITE_OK) {
                    throw binding_error(db_->last_error());
                }
                return *this;
            }
            statement &statement::bind(size_t index, const sql_blob &value)
            {
                if (!is_valid()) {
                    throw binding_error("statment invalid");
                }
                if (sqlite3_bind_blob(stmt_.get(), index, value.value(), value.size(), value.is_transient() ? SQLITE_TRANSIENT : SQLITE_STATIC) !=
                    SQLITE_OK) {
                    throw binding_error(db_->last_error());
                }
                return *this;
            }

            statement &statement::bind(size_t index, const sql_null_type &value)
            {
                if (!is_valid()) {
                    throw binding_error("statment invalid");
                }
                if (sqlite3_bind_null(stmt_.get(), index) != SQLITE_OK) {
                    throw binding_error(db_->last_error());
                }
                return *this;
            }
            statement &statement::bind(size_t index, const sql_time &value)
            {
                if (!is_valid()) {
                    throw binding_error("statment invalid");
                }

                auto tstr = value.to_string();

                if (sqlite3_bind_text(stmt_.get(), index, tstr.c_str(), tstr.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
                    throw binding_error(db_->last_error());
                }
                return *this;
            }

            statement &statement::bind(const string &name, const sql_value &value)
            {
                if (!is_valid()) {
                    throw binding_error("statment invalid");
                }

                int index = sqlite3_bind_parameter_index(stmt_.get(), name.c_str());

                if (index <= 0) {
                    throw binding_error("No such named parameter '" + name + "'");
                }

                bind_value(index, value);
                return *this;
            }

            statement::resultset_type statement::results()
            {
                if (db_ == nullptr) {
                    throw database_exception("sqlite statement results invalid database");
                }
                if (db_->cache_level() == cache::ResultSet)
                    return resultset_type(make_shared<cached_resultset>(db_, stmt_));
                else
                    return resultset_type(make_shared<resultset>(db_, stmt_));
            }

            bool statement::result()
            {
                if (!is_valid()) {
                    log::warn("sqlite statement result invalid");
                    return false;
                }
                return sqlite3_step(stmt_.get()) == SQLITE_DONE;
            }

            void statement::finish()
            {
                stmt_ = nullptr;
            }

            void statement::reset()
            {
                if (!is_valid()) {
                    log::warn("sqlite statement reset invalid");
                    return;
                }
                if (sqlite3_reset(stmt_.get()) != SQLITE_OK) {
                    throw database_exception(db_->last_error());
                }
            }

            long long statement::last_insert_id()
            {
                if (db_ == nullptr) {
                    return 0;
                }
                return db_->last_insert_id();
            }
        }
    }
}

#endif

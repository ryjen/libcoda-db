/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#include "sqldb.h"
#include "base_query.h"
#include "exception.h"
#include "resultset.h"

namespace arg3
{
    namespace db
    {
        ARG3_IMPLEMENT_EXCEPTION(database_exception, std::exception);

        ARG3_IMPLEMENT_EXCEPTION(no_such_column_exception, database_exception);

        ARG3_IMPLEMENT_EXCEPTION(record_not_found_exception, database_exception);

        ARG3_IMPLEMENT_EXCEPTION(binding_error, database_exception);

        sqldb::sqldb(const string &name) : db_(NULL), filename_(name)
        {
        }

        sqldb::sqldb(const sqldb &other) : db_(other.db_), filename_(other.filename_)
        {

        }

        sqldb::sqldb(sqldb &&other) : db_(std::move(other.db_)), filename_(std::move(other.filename_))
        {

        }

        sqldb &sqldb::operator=(const sqldb &other)
        {
            if (this != &other)
            {
                db_ = other.db_;
                filename_ = other.filename_;
            }

            return *this;
        }

        sqldb &sqldb::operator=(sqldb &&other)
        {
            if (this != &other)
            {
                db_ = std::move(other.db_);
                filename_ = std::move(other.filename_);
            }

            return *this;
        }

        sqldb::~sqldb()
        {
        }

        string sqldb::filename() const
        {
            return filename_;
        }

        void sqldb::set_filename(const string &value)
        {
            filename_ = value;
        }

        void sqldb::open()
        {

            if (db_ != NULL) return;

            if (sqlite3_open(filename_.c_str(), &db_) != SQLITE_OK)
                throw database_exception(last_error());
        }

        bool sqldb::is_open() const
        {
            return db_ != NULL;
        }

        void sqldb::close()
        {
            if (db_ == NULL) return;

            sqlite3_close(db_);
            db_ = NULL;
        }

        string sqldb::last_error() const
        {
            ostringstream buf;

            buf << sqlite3_errcode(db_);
            buf << ": " << sqlite3_errmsg(db_);

            return buf.str();
        }

        sqlite3_int64 sqldb::last_insert_id() const {

            return sqlite3_last_insert_rowid(db_);
        }

        int sqldb::last_number_of_changes() const {
            return sqlite3_changes(db_);
        }

        resultset sqldb::execute(const string &sql)
        {
            sqlite3_stmt *stmt;

            if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK)
                throw database_exception(last_error());

            resultset set(this, stmt);

            set.step();

            return set;
        }
    }
}


namespace std
{
    string to_string(const arg3::db::base_query &query)
    {
        return query.to_string();
    }
}

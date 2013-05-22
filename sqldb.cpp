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

        void sqldb::setFilename(const string &value)
        {
            filename_ = value;
        }

        void sqldb::open()
        {

            if (db_ != NULL) return;

            if (sqlite3_open(filename_.c_str(), &db_) != SQLITE_OK)
                throw database_exception();
        }

        bool sqldb::isOpen() const
        {
            return db_ != NULL;
        }

        void sqldb::close()
        {
            if (db_ == NULL) return;

            sqlite3_close(db_);
            db_ = NULL;
        }

        string sqldb::lastError() const
        {
            ostringstream buf;

            buf << sqlite3_errcode(db_);
            buf << ": " << sqlite3_errmsg(db_);

            return buf.str();
        }

        resultset sqldb::execute(const string &sql)
        {
            sqlite3_stmt *stmt;

            if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK)
                throw database_exception();

            resultset set(stmt);

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

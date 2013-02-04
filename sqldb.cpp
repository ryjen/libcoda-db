#include "sqldb.h"
#include "../collections/collections.h"
#include "../strings/strings.h"
#include "select_query.h"

//using namespace arg3::collections;

namespace arg3
{
    namespace db
    {

        sqldb::sqldb(const string &name) : mDb(NULL), mFileName(name)
        {
        }

        sqldb::sqldb(const sqldb &other) : mDb(other.mDb), mFileName(other.mFileName)
        {

        }

        sqldb &sqldb::operator=(const sqldb &other) 
        {
        	if(this != &other) {
        		mDb = other.mDb;
        		mFileName = other.mFileName;
        	}

        	return *this;
        }

        sqldb::~sqldb() {
        }

        void sqldb::open() {

        	if(mDb != NULL) return;

            cout << "opening db " << mFileName << endl;

            if (sqlite3_open(mFileName.c_str(), &mDb) != SQLITE_OK)
                throw database_exception();
        }

        void sqldb::close() {
			if(mDb == NULL) return;

        	sqlite3_close(mDb);
        	mDb = NULL;
        }

        select_query sqldb::select(const columnset &columns, const string &tablename, const string &where, 
        	const string &orderBy, const string &limit, const string &groupBy) const
        {
            select_query query(*this, tablename, columns);

            query.where(where);

            query.orderBy(orderBy);

            query.limit(limit);

            query.groupBy(groupBy);

            return query;
        }

        select_query sqldb::select(const string &tablename, const string &where, 
        	const string &orderBy, const string &limit, const string &groupBy) const
        {	
            return select({}, tablename, where, orderBy, limit, groupBy);
        }

        string sqldb::last_error() const {
        	ostringstream buf;

        	buf << sqlite3_errcode(mDb);
        	buf << ": " << sqlite3_errmsg(mDb);

        	return buf.str();
        }

        void sqldb::execute(const string &sql, sql_callback callback) {
        	if(sqlite3_exec(mDb, sql.c_str(), callback, this, NULL) != SQLITE_OK)
        		throw database_exception();
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

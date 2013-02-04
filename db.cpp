#include "db.h"
#include "../collections/collections.h"
#include "../strings/strings.h"

//using namespace arg3::collections;

namespace std
{
    string to_string(const arg3::db::base_query &query)
    {
        return query.to_string();
    }
}

namespace arg3
{
    namespace db
    {

    	///////////////// SQLDB 

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
        	close();
        }

        void sqldb::open() {

        	if(mDb != NULL) return;

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

        void sqldb::execute(const string &sql, exec_callback callback) {
        	if(sqlite3_exec(mDb, sql.c_str(), callback, this, NULL) != SQLITE_OK)
        		throw database_exception();
        }


        ///////////////// base_record

        base_record::base_record() {}

        base_record::base_record(const row &values) : mRow(values) {}

        void base_record::save() const
        {
            save_query query(*this);

            query.execute();
        }

        variant base_record::get(const string &name) {
        	return mRow[name];
        }

        void base_record::set(const string &name, const variant &value) {
        	if(!name.empty())
	        	mRow[name] = value;
        }

        void base_record::unset(const string &name) {
        	mRow.erase(name);
        }

        /////////////////// base_query

        base_query::base_query(const sqldb &db, const string &tableName, const columnset &columns) 
         : mDb(db.mDb), mStmt(NULL), mTableName(tableName), mColumns(columns) 
         {}

        void base_query::prepare()
        {
        	if(mStmt != NULL) return;

        	string sql = to_string();

            if (sqlite3_prepare_v2(mDb, sql.c_str(), -1, &mStmt, NULL) != SQLITE_OK)
                throw database_exception();
        }

        base_query &base_query::bind(size_t index, const string &value)
        {
        	prepare();

            if (sqlite3_bind_text(mStmt, index, value.c_str(), -1, NULL) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        base_query &base_query::bind(size_t index, int value)
        {
        	prepare();

            if (sqlite3_bind_int(mStmt, index, value) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        base_query &base_query::bind(size_t index, sqlite3_int64 value)
        {
        	prepare();

            if (sqlite3_bind_int64(mStmt, index, value) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        base_query &base_query::bind(size_t index)
        {
        	prepare();

            if (sqlite3_bind_null(mStmt, index) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        base_query &base_query::bind(size_t index, double value)
        {
        	prepare();

            if (sqlite3_bind_double(mStmt, index, value) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        base_query &base_query::bind(size_t index, const void *data, size_t size)
        {
        	prepare();

            if (sqlite3_bind_blob(mStmt, index, data, size, NULL) != SQLITE_OK)
                throw database_exception();
            return *this;
        }

        ////////////////////// select_query

        select_query::select_query(const base_record &record) : base_query(record.db(), record.tableName(), record.columns())
        {}
        
        select_query::select_query(const sqldb &db, const string &tableName, 
        	const columnset &columns) : base_query(db, tableName, columns)
        {}

        string select_query::to_string() const
        {
            ostringstream buf;

            buf << "SELECT ";

            buf << (mColumns.size() == 0 ? "*" : join(mColumns));

            buf << " FROM " << mTableName;

            if (!mWhere.empty())
            {
                buf << " WHERE " << mWhere;
            }

            if (!mOrderBy.empty())
            {
                buf << " ORDER BY " << mOrderBy;
            }

            if (!mLimit.empty())
            {
                buf << " LIMIT " << mLimit;
            }

            if (!mGroupBy.empty())
            {
                buf << " GROUP BY " << mGroupBy;
            }

            return buf.str();
        }

        rowset select_query::execute()
        {
        	prepare();

            int res = sqlite3_step(mStmt);

            rowset set;

            while (res == SQLITE_ROW)
            {
                row row;

                size_t count = sqlite3_column_count(mStmt);

                for (size_t i = 0; i < count; i++)
                {
                    string name = sqlite3_column_name(mStmt, i);

                    row[name] = reinterpret_cast<const char *>(sqlite3_column_text(mStmt, i));
                }

                set.push_back(row);

                res = sqlite3_step(mStmt);
            }

            return set;
        }


        ////////////////////// save_query

        save_query::save_query(const base_record &record) : base_query(record.db(), record.tableName(), record.columns()) {
        }

        save_query::save_query(const sqldb &db, const string &tableName, 
        	const columnset &columns) : base_query(db, tableName, columns)
        {}

        string save_query::to_string() const
        {
            ostringstream buf;

            buf << "REPLACE INTO " << mTableName;

            if (mColumns.size() > 0)
            {
                buf << "(";

                buf << join(mColumns);

                buf << ") VALUES (";

                buf << join("?", mColumns.size());

                buf << ")";
            }
            else
            {
                buf << " DEFAULT VALUES";
            }

            cout << buf.str() << endl;

            return buf.str();
        }

        bool save_query::execute()
        {
        	prepare();

            int res = sqlite3_step(mStmt);

            sqlite3_finalize(mStmt);

            return res == SQLITE_OK;
        }
    }
}
#ifndef _ARG3_DB_H_
#define _ARG3_DB_H_

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <exception>
#include <sqlite3.h>

#include "../variant/variant.h"

using namespace std;

// arg3::db db;

// db.select(data.getSchema().columns());

// db.from(data.getSchema().tableName());

// db.where("id = ?", id);

// db.limit(1);

namespace arg3
{
    namespace db
    {

        typedef map<string, variant> row;

        typedef vector<row> rowset;

        typedef vector<string> columnset;

        typedef int (*exec_callback)(void *, int, char **, char **);

        class database_exception : public exception
        {

        };

        class select_query;

        class save_query;

        class base_record;

        class sqldb
        {
        friend class base_query;
        private:
            sqlite3 *mDb;
            string mFileName;
        public:
            sqldb(const string &name = "arg3.db");
            sqldb(const sqldb &other);
            sqldb &operator=(const sqldb &other);
            ~sqldb();

            void open();
            void close();

            select_query select(const columnset &columns, const string &tablename,
            	const string &where = "", const string &orderBy = "", const string &limit = "", const string &groupBy = "") const;
            
            select_query select(const string &tablename,
            	const string &where = "", const string &orderBy = "", const string &limit = "", const string &groupBy = "") const;

            void execute(const string &sql, exec_callback = NULL);

            string last_error() const;
        };

        class base_record
        {
        private:
 			row mRow;
        public:	
        	base_record();

        	base_record(const row &values);

        	virtual columnset columns() const = 0;

        	virtual sqldb db() const = 0;

        	virtual string tableName() const = 0;

        	void save() const;

        	variant get(const string &name);

        	void set(const string &name, const variant &value);

        	void unset(const string &name);
        };

        class base_query
        {
            friend class sqldb;
        protected:
            sqlite3 *mDb;
            sqlite3_stmt *mStmt;
            string mTableName;
            columnset mColumns;

            void prepare();

        public:
        	base_query(const sqldb &db, const string &tableName, const columnset &columns);

            virtual string to_string() const = 0;

            base_query &bind(size_t index, const string &value);

            base_query &bind(size_t index, int value);

            base_query &bind(size_t index, sqlite3_int64 value);

            base_query &bind(size_t index);

            base_query &bind(size_t index, double value);

            base_query &bind(size_t index, const void *data, size_t size);
        };

        class select_query : public base_query
        {
        private:
            string mWhere;
            string mLimit;
            string mOrderBy;
            string mGroupBy;
        public:
        	select_query(const base_record &record);

        	select_query(const sqldb &db, const string &tableName, const columnset &columns);

        	void where(const string &value) { mWhere = value; }

        	void limit(const string &value) { mLimit = value; }

        	void orderBy(const string &value) { mOrderBy = value; }

        	void groupBy(const string &value) { mGroupBy = value; }

            string to_string() const;

            rowset execute();
        };

        class save_query : public base_query
        {
        public:
        	save_query(const base_record &record);

			save_query(const sqldb &db, const string &tableName, const columnset &columns);

            string to_string() const;

            bool execute();
        };

    }
}

namespace std
{
    string to_string(const arg3::db::base_query &query);
}
#endif

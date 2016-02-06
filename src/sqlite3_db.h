/*!
 * @file sqlite3_db.h
 */
#ifndef ARG3_DB_SQLITE_SQLDB_H
#define ARG3_DB_SQLITE_SQLDB_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBSQLITE3

#include <sqlite3.h>

#include "sqldb.h"

namespace arg3
{
    namespace db
    {
        namespace cache
        {
            /*!
             * level of caching results in a database
             */
            typedef enum { None, ResultSet, Row, Column } level;
        }

        /*!
         * a sqlite specific implementation of a database
         */
        class sqlite3_db : public sqldb
        {
            friend class base_query;
            friend class sqlite3_statement;

           protected:
            std::shared_ptr<sqlite3> db_;
            cache::level cacheLevel_;

           public:
            /*!
             * @param info   the connection info
             */
            sqlite3_db(const uri &info);

            /* boilerplate */
            sqlite3_db(const sqlite3_db &other);
            sqlite3_db(sqlite3_db &&other);
            sqlite3_db &operator=(const sqlite3_db &other);
            sqlite3_db &operator=(sqlite3_db &&other);
            virtual ~sqlite3_db();

            /* sqldb overrides */
            bool is_open() const;
            void open(int flags);
            void open();
            void close();
            long long last_insert_id() const;
            int last_number_of_changes() const;
            resultset execute(const std::string &sql);
            std::string last_error() const;
            std::shared_ptr<statement> create_statement();

            /*! @copydoc
             *  overriden for sqlite3 specific pragma parsing
             */
            void query_schema(const std::string &tableName, std::vector<column_definition> &columns);


            /*!
             * sets the cache level
             * @param level of caching
             */
            void set_cache_level(cache::level level);

            /*!
             * gets the cache level for this database
             * @return the level of caching
             */
            cache::level cache_level() const;
        };
    }
}

#endif

#endif

/*!
 * @file db.h
 */
#ifndef ARG3_DB_SQLITE_SQLDB_H
#define ARG3_DB_SQLITE_SQLDB_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBSQLITE3

#include <sqlite3.h>
#include "../sqldb.h"

namespace arg3
{
    namespace db
    {
        namespace sqlite
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
            class db : public sqldb
            {
                friend class statement;

               protected:
                std::shared_ptr<sqlite3> db_;
                cache::level cacheLevel_;

               public:
                /*!
                 * @param info   the connection info
                 */
                db(const uri &info);

                /* boilerplate */
                db(const db &other);
                db(db &&other);
                db &operator=(const db &other);
                db &operator=(db &&other);
                virtual ~db();

                /* sqldb overrides */
                bool is_open() const;
                void open(int flags);
                void open();
                void close();
                long long last_insert_id() const;
                int last_number_of_changes() const;
                resultset_type execute(const std::string &sql);
                std::string last_error() const;
                std::shared_ptr<statement_type> create_statement();

                /*! @copydoc
                 *  overriden for sqlite3 specific pragma parsing
                 */
                void query_schema(const std::string &tableName, std::vector<column_definition> &columns);


                /*!
                 * sets the cache level
                 * @param level of caching
                 */
                db &cache_level(cache::level level);

                /*!
                 * gets the cache level for this database
                 * @return the level of caching
                 */
                cache::level cache_level() const;
            };
        }
    }
}

#endif

#endif

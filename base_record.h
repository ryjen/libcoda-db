/*!
 * implementation of a database record
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_BASE_RECORD_H_
#define _ARG3_DB_BASE_RECORD_H_

#include "sqldb.h"
#include "select_query.h"
#include "modify_query.h"
#include "../variant/variant.h"
#include "../format/format.h"
#include "schema.h"
#include <map>

namespace arg3
{
    namespace db
    {
        class sqldb;
        class row;

        template<typename T>
        class base_record
        {
        private:
            map<string, variant> values_;
            schema schema_;

            void assert_schema()
            {

                if (!schema_.is_valid())
                    schema_.init(db(), tableName());
            }

        public:
            /*!
             * default constructor
             */
            base_record()
            {
            }

            /*!
             * construct with values from a database row
             */
            base_record(const row &values)
            {
                init(values);
            }

            /*!
             * initializes with values from a database row
             */
            void init(const row &values)
            {
                for (auto v = values.begin(); v != values.end(); v++)
                {
                    values_[v.name()] = v->to_string();
                }
            }

            bool is_valid() const
            {
                return schema_.is_valid();
            }

            /*!
             * sub classes should define the table schema here
             */
            //virtual column_definition columns() const = 0;

            /*!
             * should return the database for the record
             */
            virtual sqldb db() const = 0;

            /*!
             * should return the table name for the record
             */
            virtual string tableName() const = 0;

            const schema &schema()
            {
                assert_schema();
                return schema_;
            }

            /*!
             * saves this instance
             */
            bool save()
            {
                modify_query query(db(), tableName(), schema().column_names());

                int index = 1;

                // bind the object values
                for (auto & column : schema().columns())
                {
                    auto value = values_[column.name()];

                    switch (column.type())
                    {
                    case SQLITE_TEXT:
                        query.bind(index, value.to_string());
                        break;
                    case SQLITE_INTEGER:
                        query.bind(index, value.to_llong());
                        break;
                    case SQLITE_FLOAT:
                        query.bind(index, value.to_double());
                        break;
                    default:
                        query.bind(index);
                        break;
                    }

                    index++;
                }

                return query.execute();
            }

            /*!
             * gets a value specified by column name
             */
            variant get(const string &name)
            {
                return values_[name];
            }

            /*!
             * check for the existance of a column by name
             */
            bool has(const string &name) const
            {
                return values_.find(name) != values_.end();
            }

            /*!
             * sets a string for a column name
             */
            void set(const string &name, const variant &value)
            {
                values_[name] = value;
            }

            /*!
             * unsets / removes a column
             */
            void unset(const string &name)
            {
                values_.erase(name);
            }

            /*!
             * looks up and returns all objects of a base_record type
             */

            vector<T> findAll() const
            {
                auto query = select_query(db(), tableName(), schema().columns());

                auto results = query.execute();

                vector<T> items;

                for (auto & row : results)
                {
                    items.emplace_back(row);
                }

                return items;
            }

            T findById() const
            {
                auto query = select_query(db(), tableName(), schema().columns());

                auto params = select_query::where_clause();

                // find by primary keys
                for (auto & pk : schema_.primary_keys())
                {
                    params && (format("{0} = ?", pk));
                }

                query.where(params);

                int index = 1;

                // bind primary key values
                for (auto & pk : schema_.primary_keys())
                {
                    query.bind(index, values_[index - 1]);
                    index++;
                }

                auto results = query.execute();

                auto it = results.begin();

                if (it != results.end())
                    return *it;

                throw record_not_found_exception();
            }

            template<typename V>
            vector<T> findBy(const string &name, const V &value) const
            {
                auto query = select_query(db(), tableName(), schema().column_names());

                query.where(format("{0} = ?", name).str());

                query.bind(1, value);

                auto results = query.execute();

                vector<T> items;

                for (auto & row : results)
                {
                    items.emplace_back(row);
                }

                return items;
            }

            template<typename V>
            bool loadBy(const string &name, const V &value)
            {
                auto query = select_query(db(), tableName(), schema().column_names());

                query.where(format("{0} = ? LIMIT 1", name).str());

                query.bind(1, value);

                auto result = query.execute();

                if (!result.is_valid())
                    return false;

                init(*result);

                return true;
            }
        };
    }
}

#endif

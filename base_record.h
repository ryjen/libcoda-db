/*!
 * implementation of a database record
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_BASE_RECORD_H_
#define _ARG3_DB_BASE_RECORD_H_

#include "defines.h"
#include "sqldb.h"
#include "select_query.h"
#include "modify_query.h"
#include "../variant/variant.h"
#include "../format/format.h"

namespace arg3
{
    namespace db
    {
        class sqldb;
        class row;

        /*!
         * implementation of a query
         * @copyright ryan jennings (arg3.com), 2013 under LGPL
         */
        template<typename T>
        class base_record
        {
        private:
            map<string, variant> m_values;
        public:
            /*!
             * default constructor
             */
            base_record() {}

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

                for (row::const_iterator v = values.begin(); v != values.end(); v++)
                {
                    m_values[v.name()] = v->to_string();
                }
            }

            /*!
             * sub classes should define the table schema here
             */
            virtual column_definition columns() const = 0;

            /*!
             * should return the database for the record
             */
            virtual sqldb db() const = 0;

            /*!
             * should return the table name for the record
             */
            virtual string tableName() const = 0;

            /*!
             * saves this instance
             */
            bool save()
            {
                modify_query query(db(), tableName(), columns());

                int index = 1;
for (auto & column : columns())
                {
                    auto value = m_values[column.first];

                    switch (column.second)
                    {
                    case SQLITE_TEXT:
                        query.bind(index, value.to_string());
                        break;
                    case SQLITE_INTEGER:
                        query.bind(index, stoll(value));
                        break;
                    case SQLITE_FLOAT:
                        query.bind(index, stod(value));
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
                return m_values[name];
            }

            bool has(const string &name) const
            {
                return m_values.find(name) != m_values.end();
            }

            /*!
             * sets a string for a column name
             */
            void set(const string &name, const string &value)
            {
                m_values[name] = value;
            }

            void set(const string &name, int value)
            {
                m_values[name] = value;
            }

            /*!
             * sets an integer for a column name
             */
            void set(const string &name, long long value)
            {
                m_values[name] = value;
            }

            /*!
             * sets a double for a column name
             */
            void set(const string &name, double value)
            {
                m_values[name] = std::to_string(value);
            }

            /*!
             * sets bytes for a column name
             */
            void set(const string &name, void *data, size_t size)
            {

            }

            /*!
             * unsets / removes a column
             */
            void unset(const string &name)
            {
                m_values.erase(name);
            }

            /*!
             * looks up and returns all objects of a base_record type
             */

            vector<T> findAll() const
            {
                auto query = select_query(db(), tableName(), columns());

                auto results = query.execute();

                vector<T> items;

for (auto & row : results)
                {
                    items.emplace_back(row);
                }

                return items;
            }

            template<typename V>
            vector<T> findBy(const string &name, const V &value)
            {
                auto query = select_query(db(), tableName(), columns());

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
            void loadBy(const string &name, const V &value)
            {
                auto query = select_query(db(), tableName(), columns());

                query.where(format("{0} = ?", name).str());

                query.bind(1, value);

                auto results = query.execute();

                auto it = results.begin();

                if (it != results.end())
                    init(*it);
            }
        };
    }
}

#endif

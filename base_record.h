/*!
 * implementation of a database record
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_BASE_RECORD_H_
#define _ARG3_DB_BASE_RECORD_H_

#include "sqldb.h"
#include "select_query.h"
#include "modify_query.h"
#include "record_schema.h"
#include <memory>

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
            map<string, sql_value> values_;
            sqldb *db_;
            string tableName_;

            /*void assert_schema()
            {
                if (schema_ == nullptr)
                {
                    schema_ = record_schema::get(db(), table_name());
                }
            }*/
        public:
            /*!
             * default constructor
             */
            base_record(sqldb *db, const string &tableName) : db_(db), tableName_(tableName)
            {
            }

            base_record(sqldb *db, const string &tableName, const string &value) : db_(db), tableName_(tableName)
            {
                set(record_schema::ID_COLUMN_NAME, value);
            }

            template<typename V>
            base_record(sqldb *db, const string &tableName, V value) : db_(db), tableName_(tableName)
            {
                set(record_schema::ID_COLUMN_NAME, to_string(value));
            }

            /*!
             * construct with values from a database row
             */
            base_record(sqldb *db, const string &tableName, const row &values) : db_(db), tableName_(tableName)
            {
                init(values);
            }

            base_record(const base_record &other) : db_(other.db_), tableName_(other.tableName_), values_(other.values_)
            {}

            base_record(base_record &&other) : db_(other.db_), tableName_(std::move(other.tableName_)), values_(std::move(other.values_))
            {
            }

            virtual ~base_record()
            {

            }

            void set_id(const sql_value &value)
            {
                set(record_schema::ID_COLUMN_NAME, value);
            }

            sql_value id() const
            {
                return get(record_schema::ID_COLUMN_NAME);
            }

            base_record &operator=(const base_record &other)
            {
                if (this != &other)
                {
                    values_ = other.values_;
                    db_ = other.db_;
                    tableName_ = other.tableName_;
                }
                return *this;
            }

            base_record &operator=(base_record && other)
            {
                if (this != &other)
                {
                    values_ = std::move(other.values_);
                    db_ = other.db_;
                    tableName_ = std::move(other.tableName_);
                }
                return *this;
            }

            /*!
             * initializes with values from a database row
             */
            void init(const row &values)
            {
                for (auto v = values.begin(); v != values.end(); v++)
                {
                    set(v.name(), v->to_value());
                }
            }

            bool is_valid() const
            {
                return schema().is_valid();
            }

            /*!
             * sub classes should define the table schema here
             */
            //virtual column_definition columns() const = 0;

            /*!
             * should return the database for the record
             */
            sqldb *db() const
            {
                return db_;
            }

            /*!
             * should return the table name for the record
             */
            string table_name() const
            {
                return tableName_;
            }

            const record_schema &schema() const
            {
                //assert_schema();

                return *record_schema::get(db_, tableName_);
            }

            /*!
             * saves this instance
             */
            bool save()
            {
                modify_query query(db(), table_name(), schema().column_names());

                int index = 1;

                // bind the object values
                for (auto & column : schema().columns())
                {
                    auto value = values_[column.name()];

                    query.bind_value(index, value);

                    index++;
                }

                return query.execute();
            }

            /*!
             * gets a value specified by column name
             */
            sql_value get(const string &name) const
            {
                if (!has(name))
                    return sql_value();

                return values_.at(name);
            }

            /*!
             * check for the existance of a column by name
             */
            bool has(const string &name) const
            {
                return values_.count(name) > 0;
            }

            /*!
             * sets a string for a column name
             */
            void set(const string &name, const sql_value &value)
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

            vector<shared_ptr<T>> find_all()
            {
                auto query = select_query(db(), table_name(), schema().column_names());

                auto results = query.execute();

                /* convert sql rows to objects */
                vector<shared_ptr<T>> items;

                for (auto & row : results)
                {
                    items.push_back(make_shared<T>(row));
                }

                return items;
            }

            template<typename V>
            shared_ptr<T> find_by_id(V value)
            {
                auto query = select_query(db(), table_name(), schema().column_names());

                /*auto params = where_clause();

                // find by primary keys
                for (auto & pk : schema().primary_keys())
                {
                    params  &&(pk + " = ?");
                }*/

                query.where(string(record_schema::ID_COLUMN_NAME) + " = ?");

                /*int index = 1;

                // bind primary key values
                for (auto & c : schema().primary_keys())
                {
                    query.bind_value(index, values_[c]);
                    index++;
                }*/
                query.bind_value(1, value);

                auto results = query.execute();

                auto it = results.begin();

                if (it != results.end())
                {
                    return make_shared<T>(*it);
                }

                throw record_not_found_exception();
            }

            template<typename V>
            vector<shared_ptr<T>> find_by(const string &name, const V &value)
            {
                auto query = select_query(db(), table_name(), schema().column_names());

                query.where(name + " = ?");

                query.bind(1, value);

                auto results = query.execute();

                /* convert sql rows to objects */
                vector<shared_ptr<T>> items;

                for (auto & row : results)
                {
                    items.push_back(make_shared<T>(row));
                }

                return items;
            }

            bool refresh()
            {
                auto query = select_query(db(), table_name(), schema().column_names());

                /*auto params = where_clause();

                // find by primary keys
                for (auto & pk : schema().primary_keys())
                {
                    params  &&(pk + " = ?");
                }

                query.where(params);*/

                query.where(string(record_schema::ID_COLUMN_NAME) + " = ?");

                query.limit("1");

                /*int index = 1;

                // bind primary key values
                for (auto & c : schema().primary_keys())
                {
                    query.bind_value(index, values_[c]);
                    index++;
                }*/

                query.bind_value(1, id());

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

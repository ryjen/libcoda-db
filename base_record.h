/*!
 * implementation of a database record
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_BASE_RECORD_H_
#define _ARG3_DB_BASE_RECORD_H_

#include "sqldb.h"
#include "select_query.h"
#include "modify_query.h"
#include "../format/format.h"
#include "schema.h"
#include <map>
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
            static map<string, std::shared_ptr<schema>> schema_cache_;
            map<string, sql_value> values_;
            std::shared_ptr<schema> schema_;

            void assert_schema()
            {
                if (schema_ == nullptr)
                {
                    if(schema_cache_.count(tableName())) {
                        schema_ = schema_cache_[tableName()];
                    }
                    else  {
                        schema_cache_[tableName()] = schema_ = make_shared<db::schema>(db(), tableName());
                    }
                }
            }

        public:
            /*!
             * default constructor
             */
            base_record() : schema_(nullptr)
            {
            }

            /*!
             * construct with values from a database row
             */
            base_record(const row &values) : schema_(nullptr)
            {
                init(values);
            }

            base_record(const base_record &other) : values_(other.values_), schema_(other.schema_)
            {}

            base_record(base_record &&other) : values_(std::move(other.values_)), schema_(std::move(other.schema_))
            {
                other.schema_ = nullptr;
            }

            virtual ~base_record()
            {

            }

            base_record& operator=(const base_record &other)
            {
                if(this != &other)
                {
                    values_ = other.values_;
                    schema_ = other.schema_;
                }
                return *this;
            }

            base_record& operator=(base_record &&other)
            {
                if(this != &other)
                {
                    values_ = std::move(other.values_);
                    schema_ = std::move(other.schema_);
                    other.schema_ = nullptr;
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

            bool is_valid()
            {
                return schema().is_valid();
            }

            bool is_valid() const
            {
                return schema_ != nullptr && schema_->is_valid();
            }

            /*!
             * sub classes should define the table schema here
             */
            //virtual column_definition columns() const = 0;

            /*!
             * should return the database for the record
             */
            virtual sqldb *db() const = 0;

            /*!
             * should return the table name for the record
             */
            virtual string tableName() const = 0;

            const schema &schema()
            {
                assert_schema();
                return *schema_;
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
                if(!has(name))
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

            vector<shared_ptr<T>> findAll()
            {
                auto query = select_query(db(), tableName(), schema().column_names());

                auto results = query.execute();

                vector<shared_ptr<T>> items;

                for (auto & row : results)
                {
                    items.push_back(make_shared<T>(row));
                }

                return items;
            }

            shared_ptr<T> findById()
            {
                auto query = select_query(db(), tableName(), schema().column_names());

                auto params = where_clause();

                // find by primary keys
                for (auto & pk : schema().primary_keys())
                {
                    params && (format("{0} = ?", pk));
                }

                query.where(params);

                int index = 1;

                // bind primary key values
                for (auto & c : schema().primary_keys())
                {
                    query.bind_value(index, values_[c]);
                    index++;
                }

                auto results = query.execute();

                auto it = results.begin();

                if (it != results.end())
                {
                    return make_shared<T>(*it);
                }

                throw record_not_found_exception();
            }

            template<typename V>
            vector<shared_ptr<T>> findBy(const string &name, const V &value)
            {
                auto query = select_query(db(), tableName(), schema().column_names());

                query.where(format("{0} = ?", name).str());

                query.bind(1, value);

                auto results = query.execute();

                vector<shared_ptr<T>> items;

                for (auto & row : results)
                {
                    items.push_back(make_shared<T>(row));
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

        template <typename T>
        map<string,shared_ptr<schema>> base_record<T>::schema_cache_;
    }

}

#endif

/*!
 * implementation of a database record
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_BASE_RECORD_H_
#define _ARG3_DB_BASE_RECORD_H_

#include "select_query.h"
#include "modify_query.h"
#include "delete_query.h"
#include "schema.h"
#include <memory>

namespace arg3
{
    namespace db
    {
        class record_db;
        class row;

        template<typename T>
        class base_record
        {
        private:
            std::shared_ptr<schema> schema_;
            map<string, sql_value> values_;
            string idColumnName_;
        public:

            base_record(sqldb *db, const string &tablename, const string &idColumnName) : schema_(db->schemas().get(tablename)), idColumnName_(idColumnName)
            {
                assert(schema_ != nullptr);
            }

            base_record(std::shared_ptr<schema> schema, const string &columnName) : schema_(schema), idColumnName_(columnName)
            {
                assert(schema_ != nullptr);
            }

            template<typename V>
            base_record(std::shared_ptr<schema> schema, const string &columnName, V value) : base_record(schema, columnName)
            {
                set(idColumnName_, to_string(value));
                refresh();
            }

            template<typename V>
            base_record(sqldb *db, const string &tableName, const string &columnName, V value) : base_record(db, tableName, columnName)
            {
                set(idColumnName_, to_string(value));
                refresh();
            }

            /*!
             * construct with values from a database row
             */
            base_record(std::shared_ptr<schema> schema, const string &columnName, const row &values) : base_record(schema, columnName)
            {
                init(values);
            }

            base_record(sqldb *db, const string &tableName, const string &columnName, const row &values) : base_record(db, tableName, columnName)
            {
                init(values);
            }

            base_record(const base_record &other) : schema_(other.schema_), values_(other.values_), idColumnName_(other.idColumnName_)
            {}

            base_record(base_record &&other) : schema_(other.schema_), values_(std::move(other.values_)), idColumnName_(std::move(other.idColumnName_))
            {
            }

            virtual ~base_record()
            {

            }

            void set_id(const sql_value &value)
            {
                set(idColumnName_, value);
            }

            sql_value id() const
            {
                return get(idColumnName_);
            }

            base_record &operator=(const base_record &other)
            {
                if (this != &other)
                {
                    values_ = other.values_;
                    schema_ = other.schema_;
                    idColumnName_ = other.idColumnName_;
                }
                return *this;
            }

            base_record &operator=(base_record && other)
            {
                if (this != &other)
                {
                    values_ = std::move(other.values_);
                    schema_ = std::move(other.schema_);
                    idColumnName_ = std::move(other.idColumnName_);

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

            bool is_valid() const
            {
                return schema()->is_valid();
            }

            string id_column_name() const
            {
                return idColumnName_;
            }

            shared_ptr<schema> schema() const
            {
                //assert_schema();

                //return *record_schema::get(db_, tableName_);

                assert(schema_ != nullptr);
                return schema_;
            }

            /*!
             * saves this instance
             */
            bool save()
            {
                modify_query query(schema());

                int index = 1;

                // bind the object values
                for (auto & column : schema()->columns())
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
                auto query = select_query(schema());

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
                auto query = select_query(schema());

                query.where(idColumnName_ + " = ?");

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
                auto query = select_query(schema());

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
                return refresh_by(idColumnName_);
            }

            bool refresh_by(const string &name)
            {
                auto query = select_query(schema());

                query.where(name + " = ?");

                query.limit("1");

                query.bind_value(1, get(name));

                auto result = query.execute();

                if (!result.is_valid())
                    return false;

                init(*result);

                return true;
            }

            bool de1ete()
            {
                auto query = delete_query(schema());

                query.where(idColumnName_ + " = ?");

                query.bind_value(1, id());

                return query.execute();
            }
        };

    }

}

#endif

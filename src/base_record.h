/*!
 * implementation of a database record
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef ARG3_DB_BASE_RECORD_H
#define ARG3_DB_BASE_RECORD_H

#include "select_query.h"
#include "modify_query.h"
#include "schema.h"
#include <memory>

namespace arg3
{
    namespace db
    {
        class record_db;
        class row;

        template <typename T>
        class base_record;

        template <typename T>
        inline void find_all(const std::shared_ptr<schema> &schema, const typename  base_record<T>::callback &funk)
        {
            select_query query(schema);

            auto results = query.execute();

            if (!results.is_valid()) return;

            for (auto &row : results) {
                if (row.is_valid()) {
                    auto record = make_shared<T>();
                    record->init(row);
                    funk(record);
                }
            }
        }


        template <typename T>
        inline std::vector<std::shared_ptr<T>> find_all(const std::shared_ptr<schema> &schema)
        {
            /* convert sql rows to objects */
            std::vector<std::shared_ptr<T>> items;

            db::find_all<T>(schema, [&](std::shared_ptr<T> record) { items.push_back(record); });

            return items;
        }

        template <typename T>
        inline void find_by(const std::shared_ptr<schema> &schema, const std::string &name, const sql_value &value, const typename base_record<T>::callback &funk)
        {
            select_query query(schema);

            query.where(name + " = $1");

            query.bind_value(1, value);

            auto results = query.execute();

            if (!results.is_valid()) return;

            for (auto &row : results) {
                if (row.is_valid()) {
                    auto record = make_shared<T>();
                    record->init(row);
                    funk(record);
                }
            }
        }

        template <typename T>
        inline std::vector<std::shared_ptr<T>> find_by(const std::shared_ptr<schema> &schema, const std::string &name, const sql_value &value)
        {
            /* convert sql rows to objects */
            std::vector<shared_ptr<T>> items;

            db::find_by<T>(schema, name, value, [&](std::shared_ptr<T> record) { items.push_back(record); });

            return items;
        }

        /*!
         * an active-record (ish) pattern
         */
        template <typename T>
        class base_record
        {
           public:
            typedef arg3::db::schema schema_type;

            typedef std::function<void(std::shared_ptr<T>)> callback;

           private:
            std::shared_ptr<schema_type> schema_;
            std::unordered_map<std::string, sql_value> values_;
            std::string idColumnName_;

           public:
            /*!
             * @param db the database the record uses
             * @param tablename the table in the database to use
             * @param idColumnName the name of the id column in the table
             */
            base_record(sqldb *db, const std::string &tablename, const std::string &idColumnName)
                : schema_(db->schemas()->get(tablename)), idColumnName_(idColumnName)
            {
                if (schema_ == nullptr) {
                    throw database_exception("no schema for record " + tablename);
                }
            }

            /*!
             * @param schema the schema to operate on
             * @param columnName the name of the id column in the schema
             */
            base_record(const std::shared_ptr<schema_type> &schema, const std::string &columnName) : schema_(schema), idColumnName_(columnName)
            {
                if (schema_ == nullptr) {
                    throw database_exception("no schema for record");
                }
            }

            /*!
             * @param schema the schema to operate on
             * @param columnName the name of the column the id column in the schema
             * @param value the value of the id column
             */
            template <typename V>
            base_record(const std::shared_ptr<schema_type> &schema, const std::string &columnName, V value)
                : base_record(schema, columnName)
            {
                set(idColumnName_, value);
                refresh();  // load up from database
            }

            /*!
             * @param db the database to operate on
             * @param tableName the name of the table in the database
             * @param columnName the name of the id column in the table
             * @param value the value of the id column
             */
            template <typename V>
            base_record(sqldb *db, const std::string &tableName, const std::string &columnName, const V &value)
                : base_record(db, tableName, columnName)
            {
                set(idColumnName_, value);
                refresh();
            }

            /*!
             * construct with values from a database row
             */
            base_record(std::shared_ptr<schema_type> schema, const std::string &columnName, const row &values) : base_record(schema, columnName)
            {
                init(values);
            }

            /*!
             * construct with values from a database row
             */
            base_record(sqldb *db, const std::string &tableName, const std::string &columnName, const row &values)
                : base_record(db, tableName, columnName)
            {
                init(values);
            }

            /*!
             * copy constructor
             */
            base_record(const base_record &other) : schema_(other.schema_), values_(other.values_), idColumnName_(other.idColumnName_)
            {
            }

            /*!
             * move constructor
             */
            base_record(base_record &&other)
                : schema_(std::move(other.schema_)), values_(std::move(other.values_)), idColumnName_(std::move(other.idColumnName_))
            {
                other.schema_ = nullptr;
            }

            virtual ~base_record()
            {
            }

            /*!
             * assignment operator
             */
            base_record &operator=(const base_record &other)
            {
                values_ = other.values_;
                schema_ = other.schema_;
                idColumnName_ = other.idColumnName_;

                return *this;
            }

            /*!
             * move assignment operator
             */
            base_record &operator=(base_record &&other)
            {
                values_ = std::move(other.values_);
                schema_ = std::move(other.schema_);
                idColumnName_ = std::move(other.idColumnName_);

                other.schema_ = nullptr;

                return *this;
            }

            /*!
             * sets the id column of the record
             */
            void set_id(const sql_value &value)
            {
                set(idColumnName_, value);
            }

            /*!
             * returns the value of the id column in the record
             */
            sql_value id() const
            {
                return get(idColumnName_);
            }

            /*!
             * initializes with values from a database row
             */
            void init(const row &values)
            {
                if (!values.is_valid()) return;

                for (auto v = values.begin(); v != values.end(); v++) {
                    set(v.name(), v->to_value());
                }

                on_record_init(values);
            }

            virtual void on_record_init(const row &values)
            {
            }

            /*!
             * check if the record internals are valid
             */
            bool is_valid() const
            {
                return schema()->is_valid();
            }

            /*!
             * returns the name of the id column for this record
             */
            std::string id_column_name() const
            {
                return idColumnName_;
            }

            /*!
             * returns the schema for this record
             */
            std::shared_ptr<schema_type> schema() const
            {
                if (!schema_->is_valid()) schema_->init();

                return schema_;
            }

            bool exists() const
            {
                if (!has(idColumnName_)) {
                    return false;
                }

                select_query query(schema());

                query.where(idColumnName_ + " = $1");

                query.bind_value(1, get(idColumnName_));

                return query.count() > 0;
            }

            /*!
             * saves this instance
             */
            bool save()
            {
                size_t index = 0;
                bool rval = false;
                auto cols_to_save = available_columns();

                if (exists()) {
                    update_query query(schema(), cols_to_save);

                    query.where(idColumnName_ + " = $1");

                    index = bind(query, cols_to_save);

                    // add the where parameter
                    query.bind_value(index, get(idColumnName_));

                    rval = query.execute();
                } else {
                    insert_query query(schema(), cols_to_save);

                    bind(query, cols_to_save);

                    rval = query.execute();

                    if (rval) {
                        // set the new id
                        set(idColumnName_, query.last_insert_id());
                    }
                }

                return rval;
            }

            /*!
             * gets a value specified by column name
             */
            sql_value get(const std::string &name) const
            {
                if (!has(name)) {
                    return sql_null;
                }

                return values_.at(name);
            }

            /*!
             * check for the existance of a column by name
             */
            bool has(const std::string &name) const
            {
                return values_.size() > 0 && values_.count(name) > 0;
            }

            /*!
             * sets a string for a column name
             */
            void set(const std::string &name, const sql_value &value)
            {
                values_[name] = value;
            }

            /*!
             * unsets / removes a column
             */
            void unset(const std::string &name)
            {
                values_.erase(name);
            }

            /*!
             * looks up and returns all objects of a base_record type
             */

            std::vector<std::shared_ptr<T>> find_all()
            {
                return arg3::db::find_all<T>(schema());
            }

            void find_all(const callback &funk)
            {
                arg3::db::find_all<T>(schema(), funk);
            }

            /*!
             * finds a single record by its id column
             */
            std::shared_ptr<T> find_by_id(const sql_value &value)
            {
                select_query query(schema());

                query.where(idColumnName_ + " = $1");

                query.bind_value(1, value);

                auto results = query.execute();

                auto it = results.begin();

                if (it != results.end()) {
                    return make_shared<T>(*it);
                }

                throw record_not_found_exception();
            }

            /*!
             * find records by a column and its value
             */
            std::vector<std::shared_ptr<T>> find_by(const std::string &name, const sql_value &value)
            {
                return arg3::db::find_by<T>(schema(), name, value);
            }

            void find_by(const std::string &name, const sql_value &value, const callback &funk)
            {
                arg3::db::find_by<T>(schema(), name, value, funk);
            }

            /*!
             * refreshes from the database
             */
            bool refresh()
            {
                return refresh_by(idColumnName_);
            }

            /*!
             * refreshes by a column name
             */
            bool refresh_by(const std::string &name)
            {
                select_query query(schema());

                query.where(name + " = $1");

                query.limit("1");

                query.bind_value(1, get(name));

                auto result = query.execute();

                if (!result.next()) return false;

                init(*result);

                return true;
            }

            /*!
             * deletes this record from the database
             */
            bool de1ete()
            {
                delete_query query(schema());

                query.where(idColumnName_ + " = $1");

                query.bind_value(1, id());

                return query.execute();
            }

           private:
            std::vector<std::string> available_columns()
            {
                std::vector<std::string> columns = schema()->column_names();
                std::vector<std::string> values(columns.size());
                auto it = std::copy_if(columns.begin(), columns.end(), values.begin(), [&](const std::string &val) { return has(val); });
                values.resize(std::distance(values.begin(), it));
                return values;
            }
            size_t bind(query &query, const std::vector<std::string> &columns)
            {
                size_t index = 0;

                // bind the column values
                for (auto &column : columns) {
                    auto value = get(column);
                    query.bind_value(++index, value);
                }

                return index;
            }
        };
    }
}

#endif

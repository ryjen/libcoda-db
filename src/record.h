/*!
 * @file record.h
 * implementation of a database record (model)
 * @copyright ryan jennings (arg3.com), 2013
 */
#ifndef ARG3_DB_BASE_RECORD_H
#define ARG3_DB_BASE_RECORD_H

#include <memory>
#include <algorithm>
#include "select_query.h"
#include "insert_query.h"
#include "update_query.h"
#include "delete_query.h"
#include "schema.h"

namespace arg3
{
    namespace db
    {
        template <typename T>
        class record;

        /*!
         * finds all records for a schema
         * @param schema the schema to find records for
         * @param funk the callback function for each found record
         */
        template <typename T>
        inline void find_all(const std::shared_ptr<schema> &schema, const typename record<T>::callback &funk)
        {
            select_query query(schema);

            auto results = query.execute();

            if (!results.is_valid()) return;

            for (auto &row : results) {
                if (row.is_valid()) {
                    auto record = std::make_shared<T>(row);
                    funk(record);
                }
            }
        }

        /*!
         * finds records for a schema
         * @param schema the schema to find records for
         * @return a vector of records found
         */
        template <typename T>
        inline std::vector<std::shared_ptr<T>> find_all(const std::shared_ptr<schema> &schema)
        {
            /* convert sql rows to objects */
            std::vector<std::shared_ptr<T>> items;

            db::find_all<T>(schema, [&items](std::shared_ptr<T> record) { items.push_back(record); });

            return items;
        }

        /*!
         * finds records for a column value
         * @param schema the schema find records for
         * @param name the column name to search by
         * @param value the value of the column being searched
         * @param funk the callback function for each record found
         */
        template <typename T>
        inline void find_by(const std::shared_ptr<schema> &schema, const std::string &name, const sql_value &value,
                            const typename record<T>::callback &funk)
        {
            select_query query(schema);

            query.where(name + " = $1", value);

            auto results = query.execute();

            if (!results.is_valid()) return;

            for (auto &row : results) {
                auto record = std::make_shared<T>(row);
                funk(record);
            }
        }

        /*!
         * finds records for a column value
         * @param schema the schema find records for
         * @param name the column name to search by
         * @param value the value of the column being searched
         * @return a vector of results found
         */
        template <typename T>
        inline std::vector<std::shared_ptr<T>> find_by(const std::shared_ptr<schema> &schema, const std::string &name, const sql_value &value)
        {
            /* convert sql rows to objects */
            std::vector<std::shared_ptr<T>> items;

            db::find_by<T>(schema, name, value, [&items](std::shared_ptr<T> record) { items.push_back(record); });

            return items;
        }

        /*!
         * finds one record for a column value
         * @param schema the schema find records for
         * @param name the column name to search by
         * @param value the value of the column being searched
         * @param funk the callback function for each record found
         */
        template <typename T>
        inline void find_one(const std::shared_ptr<schema> &schema, const std::string &name, const sql_value &value,
                             const typename record<T>::callback &funk)
        {
            select_query query(schema);

            query.where(name + " = $1", value).limit("1");

            auto results = query.execute();

            if (!results.is_valid() || results.size() == 0) {
                return;
            }

            funk(std::make_shared<T>(*results.begin()));
        }

        /*!
         * finds one record for a column value
         * @param schema the schema find records for
         * @param funk the callback function for each record found
         */
        template <typename T>
        inline void find_one(const std::shared_ptr<schema> &schema, const typename record<T>::callback &funk)
        {
            select_query query(schema);

            auto results = query.execute();

            if (!results.is_valid() || results.size() == 0) {
                return;
            }

            funk(std::make_shared<T>(*results.begin()));
        }

        /*!
         * finds one record for a column value
         * @param schema the schema find records for
         * @param name the column name to search by
         * @param value the value of the column being searched
         * @return a vector of results found
         */
        template <typename T>
        inline std::shared_ptr<T> find_one(const std::shared_ptr<schema> &schema, const std::string &name, const sql_value &value)
        {
            /* convert sql rows to objects */
            std::shared_ptr<T> item;

            db::find_one<T>(schema, name, value, [&item](std::shared_ptr<T> record) { item = record; });

            return item;
        }

        /*!
         * finds one record for a column value
         * @param schema the schema find records for
         * @return a vector of results found
         */
        template <typename T>
        inline std::shared_ptr<T> find_one(const std::shared_ptr<schema> &schema)
        {
            /* convert sql rows to objects */
            std::shared_ptr<T> item;

            db::find_one<T>(schema, [&item](std::shared_ptr<T> record) { item = record; });

            return item;
        }

        /*!
         * an active-record (ish) pattern, should be used as a curiously reoccuring design pattern
         */
        template <typename T>
        class record
        {
           public:
            typedef arg3::db::schema schema_type;
            typedef std::function<void(const std::shared_ptr<T> &)> callback;
            typedef std::function<void(const std::shared_ptr<T> &, const std::shared_ptr<T> &)> comparator;

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
            record(sqldb *db, const std::string &tablename, const std::string &idColumnName)
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
            record(const std::shared_ptr<schema_type> &schema, const std::string &columnName) : schema_(schema), idColumnName_(columnName)
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
            record(const std::shared_ptr<schema_type> &schema, const std::string &columnName, V value)
                : record(schema, columnName)
            {
                set(idColumnName_, value);
                // load up from database
                if (!refresh()) {
                    throw database_exception("no record found with " + columnName + " of " + std::to_string(value));
                }
            }

            /*!
             * @param db the database to operate on
             * @param tableName the name of the table in the database
             * @param columnName the name of the id column in the table
             * @param value the value of the id column
             */
            template <typename V>
            record(sqldb *db, const std::string &tableName, const std::string &columnName, const V &value)
                : record(db, tableName, columnName)
            {
                set(idColumnName_, value);
                // load up from database
                if (!refresh()) {
                    throw database_exception("no record found with " + columnName + " of " + std::to_string(value));
                }
            }

            /*!
             * construct with values from a database row
             */
            record(const std::shared_ptr<schema_type> &schema, const std::string &columnName, const row &values) : record(schema, columnName)
            {
                init(values);
            }

            /*!
             * construct with values from a database row
             */
            record(sqldb *db, const std::string &tableName, const std::string &columnName, const row &values) : record(db, tableName, columnName)
            {
                init(values);
            }

            /*!
             * copy constructor
             */
            record(const record &other) : schema_(other.schema_), values_(other.values_), idColumnName_(other.idColumnName_)
            {
            }

            /*!
             * move constructor
             */
            record(record &&other)
                : schema_(std::move(other.schema_)), values_(std::move(other.values_)), idColumnName_(std::move(other.idColumnName_))
            {
            }

            virtual ~record()
            {
            }

            /*!
             * assignment operator
             */
            record &operator=(const record &other)
            {
                values_ = other.values_;
                schema_ = other.schema_;
                idColumnName_ = other.idColumnName_;

                return *this;
            }

            /*!
             * move assignment operator
             */
            record &operator=(record &&other)
            {
                values_ = std::move(other.values_);
                schema_ = std::move(other.schema_);
                idColumnName_ = std::move(other.idColumnName_);

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
             * @return the value of the id column in the record
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
                if (!values.is_valid()) {
                    return;
                }

                for (auto v = values.begin(); v != values.end(); ++v) {
                    set(v.name(), v->to_value());
                }

                on_record_init(values);
            }

            /*!
             * called when a record is read from the database
             * Overide in subclasses for custom loading actions
             * @param row the values read from the database
             */
            virtual void on_record_init(const row &values)
            {
            }

            /*!
             * check if the record internals are valid
             * @return true if the schema is valid
             */
            bool is_valid() const
            {
                return schema()->is_valid();
            }

            /*!
             * @return the name of the id column for this record
             */
            std::string id_column_name() const
            {
                return idColumnName_;
            }

            /*!
             * @return the schema for this record
             */
            std::shared_ptr<schema_type> schema() const
            {
                if (schema_ != nullptr && !schema_->is_valid()) {
                    schema_->init();
                }

                return schema_;
            }

            /*!
             *  @return true if a record with the id column value exists
             */
            bool exists() const
            {
                if (!has(idColumnName_)) {
                    return false;
                }

                select_query query(schema());

                query.where(idColumnName_ + " = $1", get(idColumnName_));

                return query.count() > 0;
            }

            /*!
             * saves this instance
             * @param insert_only set to true to never perform an update if the record exists
             * @return true if the save was successful
             */
            bool save()
            {
                size_t index = 0;
                bool rval = false;
                bool exists = record::exists();
                auto cols_to_save = available_columns(exists);

                if (exists) {
                    update_query query(schema(), cols_to_save);

                    query.where(idColumnName_ + " = $" + std::to_string(cols_to_save.size() + 1));

                    index = bind_columns_to_query(query, cols_to_save);

                    // add the where parameter
                    query.bind_value(++index, get(idColumnName_));

                    rval = query.execute();
                } else {
                    insert_query query(schema(), cols_to_save);

                    bind_columns_to_query(query, cols_to_save);

                    rval = query.execute();

                    if (rval) {
                        // set the new id
                        set(idColumnName_, query.last_insert_id());
                    }
                }

                return rval;
            }

            /*!
             * @param name the name of the column to get
             * @return a value specified by column name
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
             * @param name the name of the column to check
             * @return true if the column exists in the current record
             * NOTE: you may need to 'refresh' from the db to get all columns
             */
            bool has(const std::string &name) const
            {
                return values_.size() > 0 && values_.count(name) > 0;
            }

            /*!
             * sets a string for a column name
             * @param name the name of the column to set
             * @param value the value to set for the column
             */
            void set(const std::string &name, const sql_value &value)
            {
                values_[name] = value;
            }

            /*!
             * unsets / removes a column
             * @param name the name of the column to unset
             */
            void unset(const std::string &name)
            {
                values_.erase(name);
            }

            /*!
             * looks up and returns all objects of a record type
             * @return a vector of record objects of type T
             */
            std::vector<std::shared_ptr<T>> find_all() const
            {
                return arg3::db::find_all<T>(schema());
            }

            /*!
             * find all records
             * @param funk the callback function for each found record
             */
            void find_all(const callback &funk) const
            {
                arg3::db::find_all<T>(schema(), funk);
            }

            /*!
             * finds a single record by its id column
             */
            std::shared_ptr<T> find_by_id(const sql_value &value) const
            {
                select_query query(schema());

                query.where(idColumnName_ + " = $1", value);

                auto results = query.execute();

                auto it = results.begin();

                if (it != results.end()) {
                    return std::make_shared<T>(*it);
                }

                throw record_not_found_exception();
            }

            /*!
             * find records by a column and its value
             * @param name the name of the column to search by
             * @param value the value of the column to search by
             * @return a vector of found records of type T
             */
            std::vector<std::shared_ptr<T>> find_by(const std::string &name, const sql_value &value) const
            {
                return arg3::db::find_by<T>(schema(), name, value);
            }

            /*!
             * find records by a column and its value
             * @param name the name of the column to search by
             * @param value the value of the column to search by
             * @param funk the callback function for each record found
             */
            void find_by(const std::string &name, const sql_value &value, const callback &funk) const
            {
                arg3::db::find_by<T>(schema(), name, value, funk);
            }


            /*!
             * find records by a column and its value
             * @param name the name of the column to search by
             * @param value the value of the column to search by
             * @return a vector of found records of type T
             */
            std::shared_ptr<T> find_one(const std::string &name, const sql_value &value) const
            {
                return arg3::db::find_one<T>(schema(), name, value);
            }

            /*!
             * find records by a column and its value
             * @param name the name of the column to search by
             * @param value the value of the column to search by
             * @param funk the callback function for each record found
             */
            void find_one(const std::string &name, const sql_value &value, const callback &funk) const
            {
                arg3::db::find_one<T>(schema(), name, value, funk);
            }

            /*!
             * clears values set on this object
             */
            void reset()
            {
                values_.clear();
            }

            /*!
             * refreshes from the database for the value in the id column
             * @return true if successful
             */
            bool refresh()
            {
                return refresh_by(idColumnName_);
            }

            /*!
             * refreshes by a column name
             * @param name the name of the column to refresh by
             * @return true if successful
             */
            bool refresh_by(const std::string &name)
            {
                if (!has(name)) {
                    return false;
                }

                select_query query(schema());

                query.where(name + " = $1", get(name)).limit("1");

                auto result = query.execute();

                auto it = result.begin();

                if (it == result.end()) {
                    return false;
                }

                init(*it);

                return true;
            }

            /*!
             * deletes this record from the database for the value in the id column
             */
            bool de1ete() const
            {
                if (!has(idColumnName_)) {
                    return false;
                }
                delete_query query(schema());

                query.where(idColumnName_ + " = $1", id());

                return query.execute();
            }

           private:
            std::vector<std::string> available_columns(bool exists) const
            {
                std::vector<std::string> columns = schema()->column_names();
                std::vector<std::string> values(columns.size());
                auto it = std::copy_if(columns.begin(), columns.end(), values.begin(),
                                       [&](const std::string &val) { return has(val) && (exists || val != idColumnName_); });
                values.resize(std::distance(values.begin(), it));
                return values;
            }

            size_t bind_columns_to_query(query &query, const std::vector<std::string> &columns) const
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

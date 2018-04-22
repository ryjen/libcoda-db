#include "record.h"
#include "delete_query.h"
#include "insert_query.h"
#include "update_query.h"

namespace coda {
    namespace db {
        namespace base {
            /*!
             * @param schema the schema to operate on
             * @param columnName the name of the id column in the schema
             */
            record::record(const std::shared_ptr<schema_type> &schema) : schema_(schema) {
                if (schema_ == nullptr) {
                    throw database_exception("no schema for record");
                }
            }


            /*!
             * construct with values from a database row
             */
            record::record(const std::shared_ptr<schema_type> &schema, const row &values) : record(schema) {
                init(values);
            }


            /*!
             * copy constructor
             */
            record::record(const record &other) : schema_(other.schema_), values_(other.values_) {
            }


            /*!
             * move constructor
             */
            record::record(record &&other) : schema_(std::move(other.schema_)), values_(std::move(other.values_)) {
            }


            record::~record() {
            }

            /*!
             * assignment operator
             */
            record &record::operator=(const record &other) {
                values_ = other.values_;
                schema_ = other.schema_;
                return *this;
            }


            /*!
             * move assignment operator
             */
            record &record::operator=(record &&other) {
                values_ = std::move(other.values_);
                schema_ = std::move(other.schema_);
                return *this;
            }


            /*!
             * initializes with values from a database row
             */
            void record::init(const row &values) {
                if (!values.is_valid()) {
                    return;
                }

                for (auto v = values.begin(); v != values.end(); ++v) {
                    set(v.name(), v->value());
                }

                on_record_init(values);
            }


            /*!
             * called when a record is read from the database
             * Overide in subclasses for custom loading actions
             * @param row the values read from the database
             */
            void record::on_record_init(const row &values) {
            }

            /*!
             * check if the record internals are valid
             * @return true if the schema is valid
             */
            bool record::is_valid() const {
                return schema()->is_valid();
            }


            /*!
             * @return the schema for this record
             */
            std::shared_ptr<record::schema_type> record::schema() const {
                if (schema_ != nullptr && !schema_->is_valid()) {
                    schema_->init();
                }

                return schema_;
            }


            /*!
             *  @return true if a record with the id column value exists
             */
            bool record::exists() const {
                auto pk = schema()->primary_key();

                if (!has(pk)) {
                    return false;
                }

                select_query query(schema());

                query.where(op::equals(pk, get(pk)));

                return query.count() > 0;
            }


            /*!
             * saves this instance
             * @param insert_only set to true to never perform an update if the record exists
             * @return true if the save was successful
             */
            bool record::save() {
                bool rval = false;
                bool exists = record::exists();
                auto pk = schema()->primary_key();
                auto cols_to_save = available_columns(exists, pk);

                if (exists) {
                    update_query query(schema(), cols_to_save);

                    query.bind(get(cols_to_save));

                    query.where(op::equals(pk, get(pk)));

                    rval = query.execute();
                } else {
                    insert_query query(schema(), cols_to_save);

                    query.bind(get(cols_to_save));

                    rval = query.execute();

                    if (rval) {
                        // set the new id
                        set(pk, query.last_insert_id());
                    }
                }

                return rval;
            }

            /*!
             * @return the id of the record
             */
            sql_value record::id() const {
                return get(schema()->primary_key());
            }


            /*!
             * @param name the name of the column to get
             * @return a value specified by column name
             */
            sql_value record::get(const std::string &name) const {
                if (!has(name)) {
                    return sql_null;
                }

                return values_.at(name);
            }

            /*!
             * @param columns a vector of columns
             * @return a vector of values
             */
            std::vector<sql_value> record::get(const std::vector<std::string> &columns) const {
                std::vector<sql_value> values;

                for (auto &column : columns) {
                    if (has(column)) {
                        values.push_back(get(column));
                    }
                }
                return values;
            }


            /*!
             * check for the existance of a column by name
             * @param name the name of the column to check
             * @return true if the column exists in the current record
             * NOTE: you may need to 'refresh' from the db to get all columns
             */
            bool record::has(const std::string &name) const {
                return !name.empty() && values_.size() > 0 && values_.count(name) > 0;
            }

            /*!
             * @param value the id value to set
             */
            void record::set_id(const sql_value &value) {
                set(schema()->primary_key(), value);
            }

            /*!
             * sets a string for a column name
             * @param name the name of the column to set
             * @param value the value to set for the column
             */
            void record::set(const std::string &name, const sql_value &value) {
                values_[name] = value;
            }

            /*!
             * unsets / removes a column
             * @param name the name of the column to unset
             */
            void record::unset(const std::string &name) {
                values_.erase(name);
            }

            /*!
             * clears values set on this object
             */
            void record::reset() {
                values_.clear();
            }

            /*!
             * refreshes from the database for the value in the id column
             * @return true if successful
             */
            bool record::refresh() {
                return refresh_by(schema()->primary_key());
            }

            /*!
             * refreshes by a column name
             * @param name the name of the column to refresh by
             * @return true if successful
             */
            bool record::refresh_by(const std::string &name) {
                if (!has(name)) {
                    return false;
                }

                select_query query(schema());

                query.where(op::equals(name, get(name)));
                query.limit("1");

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
            bool record::remove() const {
                auto pk = schema()->primary_key();

                if (!has(pk)) {
                    return false;
                }
                delete_query query(schema());

                query.where(op::equals(pk, get(pk)));

                return query.execute();
            }

            /*!
             * @param exists true if the record exists
             * @param pk the primary key column name
             * @return a vector columns that are set on this record
             */
            std::vector<std::string> record::available_columns(bool exists, const std::string &pk) const {
                std::vector<std::string> values;
                auto columns = schema()->column_names();
                values.resize(columns.size());
                auto it = std::copy_if(columns.begin(), columns.end(), values.begin(),
                                       [&](const std::string &val) { return has(val) && (exists || val != pk); });
                values.resize(std::distance(values.begin(), it));
                return values;
            }
        }

        /*!
         * a generic record that doesn't have a type
         */
        namespace generic {
            /*!
             * @param value the id to find
             * @return the record
             */
            std::shared_ptr<record> record::find_by_id(const sql_value &value) const {
                return coda::db::find_by_id(schema(), value);
            }

            /*!
             * @param value the id to find
             * @param funk the callback function
             */
            void record::find_by_id(const sql_value &value, const callback &funk) const {
                coda::db::find_by_id(schema(), value, funk);
            }

            /*!
             * @return a vector of records
             */
            std::vector<std::shared_ptr<record>> record::find_all() const {
                return coda::db::find_all(schema());
            }

            /*!
             * @param funk the callback function
             */
            void record::find_all(const callback &funk) const {
                coda::db::find_all(schema(), funk);
            }

            /*!
             * @param values the map of columns ad values to find
             * @return a vector of found records
             */
            std::vector<std::shared_ptr<record>> record::find_by(const std::map<std::string, sql_value> &values) const {
                return coda::db::find_by(schema(), values);
            }

            /*!
             * @param name the column name to find
             * @param value the column value to find
             * @return a vector of records found
             */
            std::vector<std::shared_ptr<record>>
            record::find_by(const std::string &name, const sql_value &value) const {
                return coda::db::find_by(schema(), {{name, value}});
            }

            void record::find_by(const std::map<std::string, sql_value> &values, const callback &funk) const {
                coda::db::find_by(schema(), values, funk);
            }

            void record::find_by(const std::string &name, const sql_value &value, const callback &funk) const {
                coda::db::find_by(schema(), {{name, value}}, funk);
            }

            std::shared_ptr<record> record::find_one(const std::map<std::string, sql_value> &values) const {
                return coda::db::find_one(schema(), values);
            }

            std::shared_ptr<record> record::find_one(const std::string &name, const sql_value &value) const {
                return coda::db::find_one(schema(), {{name, value}});
            }

            void record::find_one(const std::map<std::string, sql_value> &values, const callback &funk) const {
                coda::db::find_one(schema(), values, funk);
            }

            void record::find_one(const std::string &name, const sql_value &value, const callback &funk) const {
                coda::db::find_one(schema(), {{name, value}}, funk);
            }
        }
    }
}
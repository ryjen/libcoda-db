/*!
 * @file record.h
 * implementation of a database record (model)
 * @copyright ryan jennings (ryan-jennings.net), 2013
 */
#ifndef RJ_DB_BASE_RECORD_H
#define RJ_DB_BASE_RECORD_H

#include <algorithm>
#include <memory>
#include "delete_query.h"
#include "insert_query.h"
#include "schema.h"
#include "select_query.h"
#include "session.h"
#include "update_query.h"

namespace rj
{
    namespace db
    {
        namespace base
        {
            /*!
             * base class for a record
             */
            class record
            {
               public:
                typedef rj::db::schema schema_type;

               private:
                std::shared_ptr<schema_type> schema_;
                std::unordered_map<std::string, sql_value> values_;

               public:
                /*!
                 * @param schema the schema to operate on
                 * @param columnName the name of the id column in the schema
                 */
                record(const std::shared_ptr<schema_type> &schema);

                /*!
                 * construct with values from a database row
                 */
                record(const std::shared_ptr<schema_type> &schema, const row &values);

                /*!
                 * copy constructor
                 */
                record(const record &other);

                /*!
                 * move constructor
                 */
                record(record &&other);

                virtual ~record();

                /*!
                 * assignment operator
                 */
                record &operator=(const record &other);

                /*!
                 * move assignment operator
                 */
                record &operator=(record &&other);

                /*!
                 * initializes with values from a database row
                 */
                void init(const row &values);

                /*!
                 * called when a record is read from the database
                 * Overide in subclasses for custom loading actions
                 * @param row the values read from the database
                 */
                virtual void on_record_init(const row &values);

                /*!
                 * check if the record internals are valid
                 * @return true if the schema is valid
                 */
                bool is_valid() const;

                /*!
                 * @return the schema for this record
                 */
                std::shared_ptr<schema_type> schema() const;

                /*!
                 *  @return true if a record with the id column value exists
                 */
                bool exists() const;

                /*!
                 * saves this instance
                 * @param insert_only set to true to never perform an update if the record exists
                 * @return true if the save was successful
                 */
                bool save();

                sql_value id() const;

                /*!
                 * @param name the name of the column to get
                 * @return a value specified by column name
                 */
                sql_value get(const std::string &name) const;

                /*!
                 * get a range of column values from the record
                 * @param a vector of column names
                 * @return a vector of values
                 */
                std::vector<sql_value> get(const std::vector<std::string> &columns) const;

                /*!
                 * check for the existance of a column by name
                 * @param name the name of the column to check
                 * @return true if the column exists in the current record
                 * NOTE: you may need to 'refresh' from the db to get all columns
                 */
                bool has(const std::string &name) const;

                /*!
                 * sets the id for this record
                 * @param the id value
                 */
                void set_id(const sql_value &value);

                /*!
                 * sets a string for a column name
                 * @param name the name of the column to set
                 * @param value the value to set for the column
                 */
                void set(const std::string &name, const sql_value &value);

                /*!
                 * unsets / removes a column
                 * @param name the name of the column to unset
                 */
                void unset(const std::string &name);

                /*!
                 * clears values set on this object
                 */
                void reset();

                /*!
                 * refreshes from the database for the value in the id column
                 * @return true if successful
                 */
                bool refresh();

                /*!
                 * refreshes by a column name
                 * @param name the name of the column to refresh by
                 * @return true if successful
                 */
                bool refresh_by(const std::string &name);

                /*!
                 * deletes this record from the database for the value in the id column
                 */
                bool de1ete() const;

               private:
                std::vector<std::string> available_columns(bool exists, const std::string &pk) const;
            };
        }

        template <typename T, typename = std::enable_if<std::is_class<T>::value>>
        class record_finder
        {
           public:
            typedef std::function<void(const std::shared_ptr<T> &)> callback;

            virtual std::shared_ptr<T> find_by_id(const sql_value &value) const = 0;

            virtual void find_by_id(const sql_value &value, const callback &funk) const = 0;

            /*!
             * looks up and returns all objects of a record type
             * @return a vector of record objects of type T
             */
            virtual std::vector<std::shared_ptr<T>> find_all() const = 0;

            /*!
             * find all records
             * @param funk the callback function for each found record
             */
            virtual void find_all(const callback &funk) const = 0;

            /*!
             * find records by a column and its value
             * @param name the name of the column to search by
             * @param value the value of the column to search by
             * @return a vector of found records of type T
             */
            virtual std::vector<std::shared_ptr<T>> find_by(const std::map<std::string, sql_value> &values) const = 0;

            /*!
             * find records by a column and its value
             * @param name the name of the column to search by
             * @param value the value of the column to search by
             * @return a vector of found records of type T
             */
            virtual std::vector<std::shared_ptr<T>> find_by(const std::string &name, const sql_value &value) const = 0;

            /*!
             * find records by a column and its value
             * @param name the name of the column to search by
             * @param value the value of the column to search by
             * @param funk the callback function for each record found
             */
            virtual void find_by(const std::map<std::string, sql_value> &values, const callback &funk) const = 0;

            /*!
             * find records by a column and its value
             * @param name the name of the column to search by
             * @param value the value of the column to search by
             * @param funk the callback function for each record found
             */
            virtual void find_by(const std::string &name, const sql_value &value, const callback &funk) const = 0;

            /*!
             * find records by a column and its value
             * @param name the name of the column to search by
             * @param value the value of the column to search by
             * @return a vector of found records of type T
             */
            virtual std::shared_ptr<T> find_one(const std::map<std::string, sql_value> &values) const = 0;

            /*!
             * find records by a column and its value
             * @param name the name of the column to search by
             * @param value the value of the column to search by
             * @return a vector of found records of type T
             */
            virtual std::shared_ptr<T> find_one(const std::string &name, const sql_value &value) const = 0;

            /*!
             * find records by a column and its value
             * @param name the name of the column to search by
             * @param value the value of the column to search by
             * @param funk the callback function for each record found
             */
            virtual void find_one(const std::map<std::string, sql_value> &values, const callback &funk) const = 0;

            /*!
             * find records by a column and its value
             * @param name the name of the column to search by
             * @param value the value of the column to search by
             * @param funk the callback function for each record found
             */
            virtual void find_one(const std::string &name, const sql_value &value, const callback &funk) const = 0;
        };

        namespace generic
        {
            /*!
             * a generic record with no type specification
             */
            class record : public base::record, public record_finder<record>
            {
               public:
                typedef std::function<void(const std::shared_ptr<record> &)> callback;

                using base::record::record;

                /*!
                 * find a record by id using this objects schema
                 * @param value the id value
                 * @return a found record or nullptr
                 */
                std::shared_ptr<record> find_by_id(const sql_value &value) const;

                /*!
                 * find a record by id using this objects schema
                 * @param value the id value
                 * @param funk the callback function
                 */
                void find_by_id(const sql_value &value, const record::callback &funk) const;

                /*!
                 * looks up and returns all objects of a record type
                 * @return a vector of record objects of type T
                 */
                std::vector<std::shared_ptr<record>> find_all() const;

                /*!
                 * find all records
                 * @param funk the callback function for each found record
                 */
                void find_all(const record::callback &funk) const;

                /*!
                 * find records by a column and its value
                 * @param values a map of columns and values to find
                 * @return a vector of found records of type T
                 */
                std::vector<std::shared_ptr<record>> find_by(const std::map<std::string, sql_value> &values) const;

                /*!
                 * find records by a column and its value
                 * @param name the name of the column to search by
                 * @param value the value of the column to search by
                 * @return a vector of found records of type T
                 */
                std::vector<std::shared_ptr<record>> find_by(const std::string &name, const sql_value &value) const;

                /*!
                 * find records by a column and its value
                 * @param values a map of columns and values to find
                 * @param funk the callback function for each record found
                 */
                void find_by(const std::map<std::string, sql_value> &values, const callback &funk) const;

                /*!
                 * find records by a column and its value
                 * @param name the name of the column to search by
                 * @param value the value of the column to search by
                 * @param funk the callback function for each record found
                 */
                void find_by(const std::string &name, const sql_value &value, const callback &funk) const;

                /*!
                 * find records by a column and its value
                 * @param values a map of columns and values to find
                 * @return a vector of found records of type T
                 */
                std::shared_ptr<record> find_one(const std::map<std::string, sql_value> &values) const;

                /*!
                 * find records by a column and its value
                 * @param name the name of the column to search by
                 * @param value the value of the column to search by
                 * @return a vector of found records of type T
                 */
                std::shared_ptr<record> find_one(const std::string &name, const sql_value &value) const;

                /*!
                 * find records by a column and its value
                 * @param values a map of columns and values to find
                 * @param funk the callback function for each record found
                 */
                void find_one(const std::map<std::string, sql_value> &values, const callback &funk) const;

                /*!
                 * find records by a column and its value
                 * @param name the name of the column to search by
                 * @param value the value of the column to search by
                 * @param funk the callback function for each record found
                 */
                void find_one(const std::string &name, const sql_value &value, const callback &funk) const;
            };
        }

        template <typename T, typename = std::enable_if<std::is_class<T>::value>>
        class record;

        /*!
         * finds records for a column value
         * @param schema the schema find records for
         * @param name the column name to search by
         * @param value the value of the column being searched
         * @param funk the callback function for each record found
         */
        template <typename T, typename = std::enable_if<std::is_base_of<base::record, T>::value>>
        inline void find_by(const std::shared_ptr<schema> &schema, const std::map<std::string, sql_value> &values,
                            const std::function<void(const std::shared_ptr<T> &)> &funk)
        {
            select_query query(schema);

            for (auto &it : values) {
                query.where() and op::equals(it.first, it.second);
            }

            auto results = query.execute();

            if (!results.is_valid()) return;

            for (auto &row : results) {
                auto record = std::make_shared<T>(schema);
                record->init(row);
                funk(record);
            }
        }

        template <typename T, typename = std::enable_if<std::is_base_of<base::record, T>::value>>
        inline void find_by(const std::shared_ptr<schema> &schema, const std::string &name, const sql_value &value,
                            const std::function<void(const std::shared_ptr<T> &)> &funk)
        {
            return find_by<T>(schema, {{name, value}}, funk);
        }

        /*!
         * finds generic records by a column
         * @param schema the schema to find records for
         * @param value the column value to find
         * @param funk the callback function
         */
        inline void find_by(const std::shared_ptr<schema> &schema, const std::string &name, const sql_value &value,
                            const std::function<void(const std::shared_ptr<generic::record> &)> &funk)
        {
            find_by<generic::record>(schema, {{name, value}}, funk);
        }
        /*!
         * finds generic records by a column
         * @param schema the schema to find records for
         * @param value the column value to find
         * @param funk the callback function
         */
        inline void find_by(const std::shared_ptr<schema> &schema, const std::map<std::string, sql_value> &values,
                            const std::function<void(const std::shared_ptr<generic::record> &)> &funk)
        {
            find_by<generic::record>(schema, values, funk);
        }

        /*!
         * finds records for a column value
         * @param schema the schema find records for
         * @param name the column name to search by
         * @param value the value of the column being searched
         * @return a vector of results found
         */
        template <typename T, typename = std::enable_if<std::is_base_of<base::record, T>::value>>
        inline std::vector<std::shared_ptr<T>> find_by(const std::shared_ptr<schema> &schema, const std::map<std::string, sql_value> &values)
        {
            /* convert sql rows to objects */
            std::vector<std::shared_ptr<T>> items;

            db::find_by<T>(schema, values, [&items](std::shared_ptr<T> record) { items.push_back(record); });

            return items;
        }

        template <typename T, typename = std::enable_if<std::is_base_of<base::record, T>::value>>
        inline std::vector<std::shared_ptr<T>> find_by(const std::shared_ptr<schema> &schema, const std::string &name, const sql_value &value)
        {
            return find_by<T>(schema, {{name, value}});
        }

        /*!
         * find generic records by a column value
         * @param schema the schema to find record for
         * @param name the name of the column to find
         * @param value the value of the column to find
         * @return a vector of generic record objects
         */
        inline std::vector<std::shared_ptr<generic::record>> find_by(const std::shared_ptr<schema> &schema, const std::string &name,
                                                                     const sql_value &value)
        {
            return find_by<generic::record>(schema, {{name, value}});
        }
        /*!
         * find generic records by a column value
         * @param schema the schema to find record for
         * @param name the name of the column to find
         * @param value the value of the column to find
         * @return a vector of generic record objects
         */
        inline std::vector<std::shared_ptr<generic::record>> find_by(const std::shared_ptr<schema> &schema,
                                                                     const std::map<std::string, sql_value> &values)
        {
            return find_by<generic::record>(schema, values);
        }

        /*!
         * finds all records for a schema
         * @param schema the schema to find records for
         * @param funk the callback function for each found record
         */
        template <typename T, typename = std::enable_if<std::is_base_of<base::record, T>::value>>
        inline void find_all(const std::shared_ptr<schema> &schema, const std::function<void(const std::shared_ptr<T> &)> &funk)
        {
            find_by<T>(schema, {}, funk);
        }

        /*!
         * find a generic record for a schema
         * @param schema the schema to find records for
         * @param funk the callback function
         */
        inline void find_all(const std::shared_ptr<schema> &schema, const std::function<void(const std::shared_ptr<generic::record> &)> &funk)
        {
            find_by<generic::record>(schema, {}, funk);
        }


        /*!
         * finds records for a schema
         * @param schema the schema to find records for
         * @return a vector of records found
         */
        template <typename T, typename = std::enable_if<std::is_base_of<base::record, T>::value>>
        inline std::vector<std::shared_ptr<T>> find_all(const std::shared_ptr<schema> &schema)
        {
            return find_by<T>(schema, {});
        }

        /*!
         * finds all generic records for a schema
         * @param schema the schema to find records for
         * @return a vector of record objects
         */
        inline std::vector<std::shared_ptr<generic::record>> find_all(const std::shared_ptr<schema> &schema)
        {
            return find_by<generic::record>(schema, {});
        }

        /*!
         * finds one record for a column value
         * @param schema the schema find records for
         * @param name the column name to search by
         * @param value the value of the column being searched
         * @param funk the callback function for each record found
         */
        template <typename T, typename = std::enable_if<std::is_base_of<base::record, T>::value>>
        inline void find_one(const std::shared_ptr<schema> &schema, const std::map<std::string, sql_value> &values,
                             const std::function<void(const std::shared_ptr<T> &)> &funk)
        {
            select_query query(schema);

            for (auto &kv : values) {
                query.where() and op::equals(kv.first, kv.second);
            }

            query.limit("1");

            auto results = query.execute();

            if (!results.is_valid()) {
                return;
            }

            auto it = results.begin();

            if (it == results.end()) {
                return;
            }

            auto record = std::make_shared<T>(schema);
            record->init(*it);
            funk(record);
        }
        template <typename T, typename = std::enable_if<std::is_base_of<base::record, T>::value>>
        inline void find_one(const std::shared_ptr<schema> &schema, const std::string &name, const sql_value &value,
                             const std::function<void(const std::shared_ptr<T> &)> &funk)
        {
            find_one<T>(schema, {{name, value}}, funk);
        }

        /*!
         * finds a single generic record
         * @param schema the schema to find records for
         * @param name the column name to find
         * @param value the column value to find
         * @param funk the callback function
         */
        inline void find_one(const std::shared_ptr<schema> &schema, const std::map<std::string, sql_value> &values,
                             const std::function<void(const std::shared_ptr<generic::record> &)> &funk)
        {
            find_one<generic::record>(schema, values, funk);
        }

        /*!
         * finds a single generic record
         * @param schema the schema to find records for
         * @param name the column name to find
         * @param value the column value to find
         * @param funk the callback function
         */
        inline void find_one(const std::shared_ptr<schema> &schema, const std::string &name, const sql_value &value,
                             const std::function<void(const std::shared_ptr<generic::record> &)> &funk)
        {
            find_one<generic::record>(schema, {{name, value}}, funk);
        }

        /*!
         * finds one record for a column value
         * @param schema the schema find records for
         * @param name the column name to search by
         * @param value the value of the column being searched
         * @return a vector of results found
         */
        template <typename T, typename = std::enable_if<std::is_base_of<base::record, T>::value>>
        inline std::shared_ptr<T> find_one(const std::shared_ptr<schema> &schema, const std::map<std::string, sql_value> &values)
        {
            /* convert sql rows to objects */
            std::shared_ptr<T> item;

            db::find_one<T>(schema, values, [&item](std::shared_ptr<T> record) { item = record; });

            return item;
        }
        /*!
         * finds one record for a column value
         * @param schema the schema find records for
         * @param name the column name to search by
         * @param value the value of the column being searched
         * @return a vector of results found
         */
        template <typename T, typename = std::enable_if<std::is_base_of<base::record, T>::value>>
        inline std::shared_ptr<T> find_one(const std::shared_ptr<schema> &schema, const std::string &name, const sql_value &value)
        {
            return find_one<T>(schema, {{name, value}});
        }

        inline std::shared_ptr<generic::record> find_one(const std::shared_ptr<schema> &schema, const std::map<std::string, sql_value> &values)
        {
            return find_one<generic::record>(schema, values);
        }
        inline std::shared_ptr<generic::record> find_one(const std::shared_ptr<schema> &schema, const std::string &name, const sql_value &value)
        {
            return find_one<generic::record>(schema, {{name, value}});
        }

        template <typename T, typename = std::enable_if<std::is_base_of<base::record, T>::value>>
        inline void find_by_id(const std::shared_ptr<schema> &schema, const sql_value &value,
                               const std::function<void(const std::shared_ptr<T> &)> &funk)
        {
            find_one<T>(schema, {{schema->primary_key(), value}}, funk);
        }

        inline void find_by_id(const std::shared_ptr<schema> &schema, const sql_value &value,
                               const std::function<void(const std::shared_ptr<generic::record> &)> &funk)
        {
            find_one<generic::record>(schema, {{schema->primary_key(), value}}, funk);
        }

        template <typename T, typename = std::enable_if<std::is_base_of<base::record, T>::value>>
        inline std::shared_ptr<T> find_by_id(const std::shared_ptr<schema> &schema, const sql_value &value)
        {
            return find_one<T>(schema, {{schema->primary_key(), value}});
        }

        inline std::shared_ptr<generic::record> find_by_id(const std::shared_ptr<schema> &schema, const sql_value &value)
        {
            return find_one<generic::record>(schema, {{schema->primary_key(), value}});
        }

        /*!
         * an active-record (ish) pattern, should be used as a curiously reoccuring design pattern
         */
        template <typename T, typename>
        class record : public base::record, public record_finder<T>
        {
           public:
            typedef std::function<void(const std::shared_ptr<T> &)> callback;

           public:
            using base::record::record;

            std::shared_ptr<T> find_by_id(const sql_value &value) const
            {
                return rj::db::find_by_id<T>(schema(), value);
            }

            void find_by_id(const sql_value &value, const callback &funk) const
            {
                rj::db::find_by_id<T>(schema(), value, funk);
            }

            /*!
             * looks up and returns all objects of a record type
             * @return a vector of record objects of type T
             */
            std::vector<std::shared_ptr<T>> find_all() const
            {
                return rj::db::find_all<T>(schema());
            }

            /*!
             * find all records
             * @param funk the callback function for each found record
             */
            void find_all(const callback &funk) const
            {
                rj::db::find_all<T>(schema(), funk);
            }

            /*!
             * find records by a column and its value
             * @param name the name of the column to search by
             * @param value the value of the column to search by
             * @return a vector of found records of type T
             */
            std::vector<std::shared_ptr<T>> find_by(const std::map<std::string, sql_value> &values) const
            {
                return rj::db::find_by<T>(schema(), values);
            }
            /*!
             * find records by a column and its value
             * @param name the name of the column to search by
             * @param value the value of the column to search by
             * @return a vector of found records of type T
             */
            std::vector<std::shared_ptr<T>> find_by(const std::string &name, const sql_value &value) const
            {
                return rj::db::find_by<T>(schema(), {{name, value}});
            }
            /*!
             * find records by a column and its value
             * @param name the name of the column to search by
             * @param value the value of the column to search by
             * @param funk the callback function for each record found
             */
            void find_by(const std::map<std::string, sql_value> &values, const callback &funk) const
            {
                rj::db::find_by<T>(schema(), values, funk);
            }
            /*!
             * find records by a column and its value
             * @param name the name of the column to search by
             * @param value the value of the column to search by
             * @param funk the callback function for each record found
             */
            void find_by(const std::string &name, const sql_value &value, const callback &funk) const
            {
                rj::db::find_by<T>(schema(), {{name, value}}, funk);
            }

            /*!
             * find records by a column and its value
             * @param name the name of the column to search by
             * @param value the value of the column to search by
             * @return a vector of found records of type T
             */
            std::shared_ptr<T> find_one(const std::map<std::string, sql_value> &values) const
            {
                return rj::db::find_one<T>(schema(), values);
            }

            /*!
             * find records by a column and its value
             * @param name the name of the column to search by
             * @param value the value of the column to search by
             * @return a vector of found records of type T
             */
            std::shared_ptr<T> find_one(const std::string &name, const sql_value &value) const
            {
                return rj::db::find_one<T>(schema(), {{name, value}});
            }

            /*!
             * find records by a column and its value
             * @param name the name of the column to search by
             * @param value the value of the column to search by
             * @param funk the callback function for each record found
             */
            void find_one(const std::map<std::string, sql_value> &values, const callback &funk) const
            {
                rj::db::find_one<T>(schema(), values, funk);
            }

            /*!
             * find records by a column and its value
             * @param name the name of the column to search by
             * @param value the value of the column to search by
             * @param funk the callback function for each record found
             */
            void find_one(const std::string &name, const sql_value &value, const callback &funk) const
            {
                rj::db::find_one<T>(schema(), {{name, value}}, funk);
            }
        };
    }
}

#endif

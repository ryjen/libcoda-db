/*!
 * implementation of a database record
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef _ARG3_DB_BASE_RECORD_H_
#define _ARG3_DB_BASE_RECORD_H_

#include "defines.h"
#include "sqldb.h"
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
        class base_record
        {
        private:
            map<string, variant> m_values;
        public:
            /*!
             * default constructor
             */
            base_record();

            /*!
             * construct with values from a database row
             */
            base_record(const row &values);

            /*!
             * initializes with values from a database row
             */
            void init(const row &values);

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
            bool save();

            /*!
             * gets a value specified by column name
             */
            variant get(const string &name);

            bool has(const string &name) const;

            /*!
             * sets a string for a column name
             */
            void set(const string &name, const string &value);

            void set(const string &name, int value);

            /*!
             * sets an integer for a column name
             */
            void set(const string &name, long long value);

            /*!
             * sets a double for a column name
             */
            void set(const string &name, double value);

            /*!
             * sets bytes for a column name
             */
            void set(const string &name, void *data, size_t size);

            /*!
             * unsets / removes a column
             */
            void unset(const string &name);

        protected:
            /*!
             * looks up and returns all objects of a base_record type
             */
            template<typename T>
            vector<T> findAll()
            {
                static_assert(is_base_of<base_record, T>::value, "template argument is not of type base_record");

                auto query = db().select(columns(), tableName());

                auto results = query.execute();

                vector<T> items;

                for (auto & row : results)
                {
                    items.emplace_back(row);
                }

                return items;
            }

            template<typename T, typename V>
            vector<T> findBy(const string &name, const V &value)
            {
                static_assert(is_base_of<base_record, T>::value, "template argument is not of type base_record");

                auto query = db().select(columns(), tableName());

                query.where(format("{0} = ?", name).str());

                query.bind(1, value);

                auto results = query.execute();

                vector<T> items;

                for(auto &row : results)
                {
                    items.emplace_back(row);
                }

                return items;
            }

            template<typename T, typename V>
            void initBy(const string &name, const V &value)
            {
                static_assert(is_base_of<base_record, T>::value, "template argument is not of type base_record");

                auto query = db().select(columns(), tableName());

                query.where(format("{0} = ?", name).str());

                query.bind(1, value);

                auto results = query.execute();

                auto it = results.begin();

                if(it != results.end())
                    init(*it);
            }

        };


        template<typename ID>
        class id_record : public base_record
        {
        protected:
            virtual string idColumnName() const  = 0;
        public:
            id_record() : base_record() {}

            id_record(ID id) : base_record() {
                initById(id);
            }
            id_record(const row &row) : base_record(row) {}

            variant getId() { return get(idColumnName()); }
            void setId(const ID &value) { set(idColumnName(), value); }

            bool is_valid() const {
                return has(idColumnName());
            }

            void initById(ID value) {
                auto query = db().select(columns(), tableName());

                query.where(format("{0} = ?", idColumnName()).str());

                auto results = query.execute();

                auto it = results.begin();
                if(it != results.end())
                    init(*it);
            }
        };
    }
}

#endif

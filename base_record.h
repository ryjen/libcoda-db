#ifndef _ARG3_DB_BASE_RECORD_H_
#define _ARG3_DB_BASE_RECORD_H_

#include "defines.h"
#include "sqldb.h"
#include "../variant/variant.h"

namespace arg3
{
    namespace db
    {
        class sqldb;
        class row;

        class base_record
        {
        private:
            map<string, variant> m_values;
        public:
            base_record();
            base_record(const row &values);

            void init(const row &values);

            virtual column_definition columns() const = 0;

            virtual sqldb db() const = 0;

            virtual string tableName() const = 0;

            bool save();

            variant get(const string &name);

            void set(const string &name, const string &value);

            void set(const string &name, long long value);

            void set(const string &name, double value);

            void set(const string &name, void *data, size_t size);

            void unset(const string &name);


            template<typename T>
            static vector<T> findAll()
            {
                static_assert(is_base_of<base_record, T>::value, "template argument is not of type base_record");

                T data;

                auto query = data.db().select(data.columns(), data.tableName());

                auto results = query.execute();

                vector<T> items;

                for (auto & row : results)
                {
                    items.push_back(T(row));
                }

                return items;
            }

        };

    }
}

#endif

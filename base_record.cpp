#include "base_record.h"
#include "modify_query.h"
#include "row.h"

namespace arg3
{
    namespace db
    {

        base_record::base_record() {}

        base_record::base_record(const row &values)
        {
            init(values);
        }

        void base_record::init(const row &values)
        {

            for (row::const_iterator v = values.begin(); v != values.end(); v++)
            {
                m_values[v.name()] = v->to_string();
            }
        }

        bool base_record::save()
        {
            modify_query query(*this);

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

        variant base_record::get(const string &name)
        {
            return m_values[name];
        }

        void base_record::set(const string &name, const string &value)
        {
            m_values[name] = value;
        }

        void base_record::set(const string &name, long long value)
        {
            m_values[name] = value;
        }

        void base_record::set(const string &name, double value)
        {
            m_values[name] = std::to_string(value);
        }

        void base_record::unset(const string &name)
        {
            m_values.erase(name);
        }

    }
}
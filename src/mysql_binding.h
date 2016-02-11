/*!
 * @file mysql_binding.h
 * utilities for binding data to a mysql query
 */
#ifndef ARG3_DB_MYSQL_BINDING_H
#define ARG3_DB_MYSQL_BINDING_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBMYSQLCLIENT

#include "sql_value.h"
#include "bindable.h"
#include <mysql/mysql.h>
#include <string>

namespace arg3
{
    namespace db
    {
        namespace mysql_data_mapper
        {
            sql_value to_value(MYSQL_BIND *binding);
            sql_value to_value(int type, const char *value, size_t length);
        }

        /*!
         * makes binding mysql queries simpler
         */
        class mysql_binding : public bindable
        {
            friend class mysql_column;

           private:
            MYSQL_BIND *value_;
            size_t size_;
            void copy_value(const MYSQL_BIND *other, size_t size);
            void clear_value();
            void clear_value(size_t index);
            bool reallocate_value(size_t index);

           public:
            /*!
             * default constructor
             */
            mysql_binding();

            /*!
             * constructor with an empty set of values
             * @param size the size of the parameter set
             */
            mysql_binding(size_t size);

            /*!
             * @param value the single binding to init with
             */
            mysql_binding(const MYSQL_BIND &value);
            /*!
             * @param value the array of values to init with
             * @param size the size of the value array
             */
            mysql_binding(MYSQL_BIND *value, size_t size);

            /*!
             * @param fields the array of fields to init with
             * @param size the size of the fields array
             */
            mysql_binding(MYSQL_FIELD *fields, size_t size);

            /* boilerplate */
            mysql_binding(const mysql_binding &other);
            mysql_binding(mysql_binding &&other);
            mysql_binding &operator=(const mysql_binding &other);
            mysql_binding &operator=(mysql_binding &&other);
            virtual ~mysql_binding();

            /*!
             * @return the size (number of bindings) of this instance
             */
            size_t size() const;

            /*!
             * @param index the index of the binding to get
             * @return the raw binding value for the given index
             */
            MYSQL_BIND *get(size_t index) const;

            /*!
             * @param index the index of the binding value
             * @return the value of the binding at the given index
             */
            sql_value to_value(size_t index) const;

            /*!
             * @param index the index of the binding
             * @return the value type of the binding at the given index
             */
            int sql_type(size_t index) const;

            /* bindable overrides */
            mysql_binding &bind(size_t index, int value);
            mysql_binding &bind(size_t index, unsigned value);
            mysql_binding &bind(size_t index, long long value);
            mysql_binding &bind(size_t index, unsigned long long value);
            mysql_binding &bind(size_t index, float value);
            mysql_binding &bind(size_t index, double value);
            mysql_binding &bind(size_t index, const std::string &value, int len = -1);
            mysql_binding &bind(size_t index, const std::wstring &value, int len = -1);
            mysql_binding &bind(size_t index, const sql_blob &value);
            mysql_binding &bind(size_t index, const sql_null_type &value);
            mysql_binding &bind(size_t index, const sql_time &value);

            /*!
             * puts values into a query before execution
             * @param stmt the statemnt to bind to
             */
            void bind_params(MYSQL_STMT *stmt) const;

            /*!
             * prepares the statement to receive results based on the bindings
             * @param stmt the raw mysql statement to bind to
             */
            void bind_result(MYSQL_STMT *stmt) const;

            /*!
             * reset all the bindings
             */
            void reset();
        };
    }
}

#endif


#endif

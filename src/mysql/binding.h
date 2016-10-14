/*!
 * @file binding.h
 * utilities for binding data to a mysql query
 */
#ifndef RJ_DB_MYSQL_BINDING_H
#define RJ_DB_MYSQL_BINDING_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBMYSQLCLIENT

#include <mysql/mysql.h>
#include <set>
#include <string>
#include <unordered_map>
#include "../bind_mapping.h"
#include "../sql_value.h"

namespace rj
{
    namespace db
    {
        namespace mysql
        {
            namespace data_mapper
            {
                sql_value to_value(MYSQL_BIND *binding);
                sql_value to_value(int type, const char *value, size_t length);
            }

            /*!
             * makes binding mysql queries simpler
             */
            class binding : public rj::db::bind_mapping
            {
                friend class mysql_column;

               private:
                MYSQL_BIND *value_;
                size_t size_;
                std::unordered_map<size_t, std::set<size_t>> indexes_;
                void copy_value(const MYSQL_BIND *other, size_t size);
                void clear_value();
                void clear_value(size_t index);
                bool reallocate_value(size_t index);
                std::set<size_t> &get_indexes(size_t index);

               public:
                /*!
                 * default constructor
                 */
                binding();

                /*!
                 * constructor with an empty set of values
                 * @param size the size of the parameter set
                 */
                binding(size_t size);

                /*!
                 * @param value the single binding to init with
                 */
                binding(const MYSQL_BIND &value);
                /*!
                 * @param value the array of values to init with
                 * @param size the size of the value array
                 */
                binding(MYSQL_BIND *value, size_t size);

                /*!
                 * @param fields the array of fields to init with
                 * @param size the size of the fields array
                 */
                binding(MYSQL_FIELD *fields, size_t size);

                /* boilerplate */
                binding(const binding &other);
                binding(binding &&other);
                binding &operator=(const binding &other);
                binding &operator=(binding &&other);
                virtual ~binding();

                /*!
                 * @return the size (number of bindings) of this instance
                 */
                size_t size() const;

                /*!
                 * @return the capcity of the storage of this instance
                 */
                size_t capacity() const;

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
                binding &bind(size_t index, const sql_value &value);
                binding &bind(const std::string &name, const sql_value &value);

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
                 * validates the sql and prepares the bindinds
                 * @param sql the sql to prepare
                 */
                std::string prepare(const std::string &sql);

                /*!
                 * reset all the bindings
                 */
                void reset();
            };
        }
    }
}

#endif


#endif

/*!
 * @file bindable.h
 * An interface for data binding
 */
#ifndef ARG3_DB_BINDABLE_H
#define ARG3_DB_BINDABLE_H

#include <unordered_map>
#include <set>
#include <regex>
#include "sql_value.h"
#include "exception.h"

namespace arg3
{
    namespace db
    {
        /*!
         * represents something that can have a sql value binded to it
         */
        class bindable
        {
           protected:
            /*!
             * bind_all override for one sql_value parameter
             * @param index the index of the binding
             * @param value the value to bind
             * @return a reference to this instance
             */
            template <typename T>
            bindable &bind_list(size_t index, const T &value)
            {
                return bind_value(index, value);
            }

            /*!
             * bind a list of a values, using the order of values as the index
             * @param index the initial index for the list
             * @param value the first value in the list
             * @param argv the remaining values
             * @return a reference to this instance
             */
            template <typename T, typename... List>
            bindable &bind_list(size_t index, const T &value, const List &... argv)
            {
                bind_value(index, value);
                bind_list(index + 1, argv...);
                return *this;
            }

           public:
            static std::regex param_regex;
            static std::regex index_regex;
            static std::regex named_regex;

            template <typename T, typename... List>
            bindable &bind_all(const T &value, const List &... argv)
            {
                return bind_list(1, value, argv...);
            }

            /*!
             * Binds a sql_value using the other bind methods
             * @param index the index of the binding
             * @param value the value of the binding
             * @return a reference to this instance
             */
            bindable &bind_value(size_t index, const sql_value &value);

            /*!
             * binds an integer value
             * @param  index the index of the binding
             * @param  value the value to bind
             * @return       a reference to this instance
             */
            virtual bindable &bind(size_t index, int value) = 0;

            /*!
             * binds an unsigned integer value
             * @param  index the index of the binding
             * @param  value the value to bind
             * @return       a reference to this instance
             */
            virtual bindable &bind(size_t index, unsigned value) = 0;

            /*!
             * binds a long long value
             * @param  index the index of the binding
             * @param  value the value to bind
             * @return       a reference to this instance
             */
            virtual bindable &bind(size_t index, long long value) = 0;

            /*!
             * binds an unsigned long long value
             * @param  index the index of the binding
             * @param  value the value to bind
             * @return       a reference to this instance
             */
            virtual bindable &bind(size_t index, unsigned long long value) = 0;

            /*!
             * binds a floating point value
             * @param  index the index of the binding
             * @param  value the value to bind
             * @return       a reference to this instance
             */
            virtual bindable &bind(size_t index, float value) = 0;

            /*!
             * binds a floating point value
             * @param  index the index of the binding
             * @param  value the value to bind
             * @return       a reference to this instance
             */
            virtual bindable &bind(size_t index, double value) = 0;

            /*!
             * binds a string value
             * @param  index the index of the binding
             * @param  value the value to bind
             * @param  len   the length of the binding (default: -1)
             * @return       a reference to this instance
             */
            virtual bindable &bind(size_t index, const std::string &value, int len = -1) = 0;

            /*!
             * binds a wide string value
             * @param  index the index of the binding
             * @param  value the value to bind
             * @param  len   the length of the binding (default: -1)
             * @return       a reference to this instance
             */
            virtual bindable &bind(size_t index, const std::wstring &value, int len = -1) = 0;

            /*!
             * binds a blob value
             * @param  index the index of the binding
             * @param  value the value to bind
             * @return       a reference to this instance
             */
            virtual bindable &bind(size_t index, const sql_blob &value) = 0;

            /*!
             * binds a null value
             * @param  index the index of the binding
             * @param  value the null value to bind
             * @return       a reference to this instance
             */
            virtual bindable &bind(size_t index, const sql_null_type &value) = 0;

            /*!
             * binds a timestamp
             * @param  index the index of the binding
             * @param  time  the value to bind
             * @return       a reference to this instance
             */
            virtual bindable &bind(size_t index, const sql_time &time) = 0;

            /*!
             * bind a named parameter
             * @param name the name of the parameter
             * @param value the value to bind
             * @return a reference to this instance
             */
            virtual bindable &bind(const std::string &name, const sql_value &value) = 0;
        };

        /*!
         * a binding that supports mapping named parameters to indexed parameters
         */
        class bind_mapping : public bindable
        {
           protected:
            typedef std::unordered_map<std::string, std::set<size_t>> type;

            void add_named_param(const std::string &name, size_t index);
            void rem_named_param(const std::string &name, size_t index);
            std::set<size_t> get_named_param_indexes(const std::string &name);

           public:
            bind_mapping();
            bind_mapping(const bind_mapping &other);
            bind_mapping(bind_mapping &&other);
            virtual ~bind_mapping();
            bind_mapping &operator=(const bind_mapping &other);
            bind_mapping &operator=(bind_mapping &&other);

            /*!
             * prepares the bindings for a sql string.  should call prepare_params()
             * @param sql the sql string with parameters
             * @return the reformatted sql
             * @throws binding_error if the sql contains mixed named and indexed parameters
             */
            virtual std::string prepare(const std::string &sql);

            bind_mapping &bind(const std::string &name, const sql_value &value);

            bool is_named() const;

            /*!
             * reset all the binding
             */
            virtual void reset();

           protected:
            type mappings_;
        };
    }
}

#endif

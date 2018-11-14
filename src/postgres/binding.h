/*!
 * @file binding.h
 * utility for binding data to a postgres query
 */
#ifndef CODA_DB_POSTGRES_BINDING_H
#define CODA_DB_POSTGRES_BINDING_H

#include "../bind_mapping.h"
#include <libpq-fe.h>
#include <string>

namespace coda {
  namespace db {
    class sql_value;

    namespace postgres {
      namespace data_mapper {
        class from_number;
        class from_value;
        sql_value to_value(Oid type, const char *value, int len);
      } // namespace data_mapper
      /*
       * utility class to simplify binding query parameters
       */
      class binding : public coda::db::bind_mapping {
        friend class column;
        friend class statement;
        friend class data_mapper::from_number;
        friend class data_mapper::from_value;

        private:
        char **values_;
        Oid *types_;
        int *lengths_;
        int *formats_;
        size_t size_;
        void copy_value(const binding &other);
        void clear_value();
        void clear_value(size_t index);
        bool reallocate_value(size_t index);

        public:
        /*!
         * default constructor
         */
        binding();
        /*!
         * zero out an array of size for binding
         */
        binding(size_t size);

        /* boilerplate */
        binding(const binding &other);
        binding(binding &&other);
        binding &operator=(const binding &other);
        binding &operator=(binding &&other);
        virtual ~binding();

        /*!
         * @return the number of bindings
         */
        size_t num_of_bindings() const noexcept;

        /*!
         * @return the current capcity of the storage
         */
        size_t capacity() const;

        /*!
         * gets the value of a binding
         * @param  index the index of the binding parameter
         * @return       the value of the binding at the given index
         */
        sql_value to_value(size_t index) const;

        /*!
         * gets the sql type of a binding
         * @param  index the index of the binding
         * @return       an integer representing the sql type
         * Note: the return value will be a postgres specific implementation
         */
        int sql_type(size_t index) const;

        /* bindable overrides */
        binding &bind(size_t index, const sql_value &value);
        binding &bind(const std::string &name, const sql_value &value);

        std::string prepare(const std::string &sql);

        /*!
         * clear and remove all bindings
         */
        void reset();
      };
    } // namespace postgres
  }   // namespace db
} // namespace coda

#endif

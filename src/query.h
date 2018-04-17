/*!
 * @file query.h
 * implementation of a query
 * @copyright ryan jennings (ryan-jennings.net), 2013
 */
#ifndef CODA_DB_QUERY_H
#define CODA_DB_QUERY_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "bindable.h"
#include "session.h"
#include "sql_generator.h"
#include "sql_value.h"

namespace coda {
    namespace db {
        class statement;

        /*!
         * abstract class
         * override to implement a query
         */
        class query : protected bindable, public sql_generator {
        public:
            typedef session session_type;

        private:
            /*!
            * ensures that the binding storage array is large enough
            * @param index the parameter index for binding
            * @returns the zero-based index suitable for the storage array
            * @throws invalid_argument if there is no specifier for the argument
            */
            size_t assert_binding_index(size_t index);

            bool dirty_;

        protected:
            std::shared_ptr<session_type> session_;
            std::shared_ptr<statement> stmt_;
            std::vector<sql_value> params_;
            std::unordered_map<std::string, sql_value> named_params_;

            /*!
             * prepares this query for the sql string
             * @param sql the sql string
             */
            void prepare(const std::string &sql);

            friend class where_builder;

            /* bindable overrides */
            bindable &bind(size_t index, const sql_value &value);

            bindable &bind(const std::string &name, const sql_value &value);

            virtual void set_modified();

        public:
            /*!
             * @param db the database to perform the query on
             * @param tableName the table to perform the query on
             */
            query(const std::shared_ptr<session_type> &sess);

            /*!
             * @param other the other query to copy from
             */
            query(const query &other) noexcept;

            /*!
             * @param other the query being moved
             */
            query(query &&other) noexcept;

            /*!
             * deconstructor
             */
            virtual ~query();

            /*!
             * resets this query for re-execution
             */
            virtual void reset();

            /*!
             * get the database in use
             * @return the database object
             */
            std::shared_ptr<query::session_type> get_session() const;

            /*!
             * @param other the other query being copied from
             */
            query &operator=(const query &other);

            /*!
             * @param other the query being moved
             */
            query &operator=(query &&other);

            size_t num_of_bindings() const;

            using bindable::bind;

            /*!
             * returns the last error the query encountered, if any
             */
            std::string last_error();

            /*!
             * tests if this query is valid
             * @return true if the internals are open and valid
             */
            virtual bool is_valid() const;
        };
    }
}

#endif

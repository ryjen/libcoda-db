#ifndef ARG3_DB_STATEMENT_H_
#define ARG3_DB_STATEMENT_H_

#include "bindable.h"
#include "resultset.h"

namespace arg3
{
    namespace db
    {
        class statement_impl : public bindable
        {
        public:
            statement_impl() = default;
            statement_impl(const statement_impl &other) = default;
            statement_impl(statement_impl &&other) = default;
            statement_impl &operator=(const statement_impl &other) = default;
            statement_impl &operator=(statement_impl && other) = default;
            virtual ~statement_impl() = default;
            virtual void prepare(const std::string &sql) = 0;
            virtual void finish() = 0;
            virtual void reset() = 0;
            virtual bool is_valid() const = 0;
            virtual resultset results() = 0;
            virtual bool result() = 0;
        };

        class statement : public bindable
        {
        private:
            shared_ptr<statement_impl> impl_;
        public:
            statement(shared_ptr<statement_impl> impl);
            statement(const statement &other);
            statement(statement &&other);
            virtual ~statement() = default;
            statement &operator=(const statement &other);
            statement &operator=(statement && other);
            void prepare(const std::string &sql);
            void finish();
            void reset();
            bool is_valid() const;
            resultset results();
            bool result();
            statement &bind(size_t index, int value);
            statement &bind(size_t index, int64_t value);
            statement &bind(size_t index, double value);
            statement &bind(size_t index, const std::string &value, int len = -1);
            statement &bind(size_t index, const sql_blob &value);
            statement &bind(size_t index, const sql_null_t &value);
            statement &bind_value(size_t index, const sql_value &v);
            statement &bind(size_t index, const void *data, size_t size, void(* pFree)(void *) = SQLITE_STATIC);
        };
    }
}

#endif


#include "statement.h"
#include "sqldb.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        statement::statement(shared_ptr<statement_impl> impl) : impl_(impl)
        {}

        statement::statement(const statement &other) : impl_(other.impl_)
        {}

        statement::statement(statement &&other) : impl_(std::move(other.impl_))
        {
            other.impl_ = nullptr;
        }
        statement &statement::operator=(const statement &other)
        {
            if (this != &other)
            {
                impl_ = other.impl_;
            }
            return *this;
        }
        statement &statement::operator=(statement && other)
        {
            if (this != &other)
            {
                impl_ = std::move(other.impl_);
                other.impl_ = nullptr;
            }
            return *this;
        }
        void statement::prepare(const std::string &sql)
        {
            impl_->prepare(sql);
        }
        void statement::finish()
        {
            impl_->finish();
        }
        void statement::reset()
        {
            impl_->reset();
        }
        bool statement::is_valid() const
        {
            return impl_->is_valid();
        }
        resultset statement::results()
        {
            return impl_->results();
        }
        bool statement::result()
        {
            return impl_->result();
        }

        statement &statement::bind(size_t index, int value)
        {
            impl_->bind(index, value);
            return *this;
        }
        statement &statement::bind(size_t index, int64_t value)
        {
            impl_->bind(index, value);
            return *this;
        }
        statement &statement::bind(size_t index, double value)
        {
            impl_->bind(index, value);
            return *this;
        }
        statement &statement::bind(size_t index, const std::string &value, int len)
        {
            impl_->bind(index, value, len);
            return *this;
        }
        statement &statement::bind(size_t index, const sql_blob &value)
        {
            impl_->bind(index, value);
            return *this;
        }
        statement &statement::bind(size_t index, const sql_null_t &value)
        {
            impl_->bind(index, value);
            return *this;
        }
        statement &statement::bind_value(size_t index, const sql_value &value)
        {
            impl_->bind_value(index, value);
            return *this;
        }
        statement &statement::bind(size_t index, const void *data, size_t size, void(* pFree)(void *))
        {
            impl_->bind(index, data, size, pFree);
            return *this;
        }
    }
}
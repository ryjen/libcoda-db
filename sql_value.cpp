#include "sql_value.h"
#include "base_query.h"
#include "sqldb.h"
#include "exception.h"
#include <sstream>

namespace std
{
    string to_string(const arg3::db::sql_value &value)
    {
        return value.to_string();
    }

    string to_string(const arg3::db::sql_null_t &value)
    {
        return "NULL";
    }

    string to_string(const arg3::db::sql_blob &value)
    {
        return value.to_string();
    }

    string to_string(const std::string &value)
    {
        return value;
    }
}

namespace arg3
{
    namespace db
    {
        const sql_null_t sql_null = sql_null_t();

        sql_blob::sql_blob(const void *ptr, size_t size, sql_blob::cleanup_method cleanup) : p_(ptr), s_(size), destruct_(cleanup)
        {}

        const void *sql_blob::ptr() const
        {
            return p_;
        }
        size_t sql_blob::size() const
        {
            return s_;
        }

        string sql_blob::to_string() const
        {
            ostringstream os;
            os << p_;
            return os.str();
        }

        bool sql_blob::operator==(const sql_blob &other) const
        {
            return p_ == other.p_ && s_ == other.s_;
        }

        sql_blob::cleanup_method sql_blob::destructor() const
        {
            return destruct_;
        }

        sql_binding_visitor::sql_binding_visitor(bindable *obj, int index) : obj_(obj), index_(index)
        {}

        void sql_binding_visitor::operator()(int value) const
        {
            obj_->bind(index_, value);
        }
        void sql_binding_visitor::operator()(int64_t value) const
        {
            obj_->bind(index_, value);
        }
        void sql_binding_visitor::operator()(double value) const
        {
            obj_->bind(index_, value);
        }
        void sql_binding_visitor::operator()(const std::string &value) const
        {
            obj_->bind(index_, value);
        }
        void sql_binding_visitor::operator()(const sql_blob &value) const
        {
            obj_->bind(index_, value);
        }
        void sql_binding_visitor::operator()(const sql_null_t &value) const
        {
            obj_->bind(index_, value);
        }

        string sql_value::to_string() const
        {
            ostringstream os;
            apply_visitor(arg3::db::ostream_sql_value_visitor(os), value_);
            return os.str();
        }
        sql_value::operator std::string() const
        {
            return to_string();
        }

        sql_value::operator int() const
        {
            return std::stod(to_string());
        }

        sql_value::operator int64_t() const
        {
            return std::stoll(to_string());
        }

        sql_value::operator double() const
        {
            return std::stod(to_string());
        }

        sql_value::operator sql_blob() const
        {
            if (!apply_visitor(sql_exists_visitor<sql_blob>(), value_))
                return sql_blob(NULL, 0);

            return thenewcpp::get<sql_blob>(value_);
        }

        std::ostream &operator<<(std::ostream &out, const sql_value &value)
        {
            apply_visitor(ostream_sql_value_visitor(out), value.value_);

            return out;
        }

        std::ostream &operator<<(std::ostream &out, const sql_null_t &value)
        {
            out << "NULL";

            return out;
        }
    }
}
